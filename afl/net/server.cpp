/**
  *  \file afl/net/server.cpp
  *  \brief Class afl::net::Server
  */

#include <memory>
#include <algorithm>
#include "afl/net/server.hpp"
#include "afl/async/controller.hpp"
#include "afl/async/operation.hpp"
#include "afl/async/receiveoperation.hpp"
#include "afl/async/sendoperation.hpp"
#include "afl/container/ptrvector.hpp"
#include "afl/net/acceptoperation.hpp"
#include "afl/net/listener.hpp"
#include "afl/net/protocolhandler.hpp"
#include "afl/net/protocolhandlerfactory.hpp"
#include "afl/net/socket.hpp"
#include "afl/string/format.hpp"
#include "afl/sys/time.hpp"
#include "afl/net/name.hpp"
#include "afl/sys/thread.hpp"

namespace {
    /* Minimum timeout.
       We round up all timeouts smaller than this.
       The idea is to avoid burning up CPU with lots of tiny timeouts that appear close to each other,
       and also make sure that if we exceed a timeout, we do so with a good margin instead of winding up the clock
       and ending up juuuust before the time due to scheduling mishaps. */
    const uint32_t MIN_TIME = 50;

    /* Name to use instead of what() for a non-std::exception exception. */
    const char UNKNOWN_EXCEPTION[] = "unidentified";
}

/** State for "stop" operation. */
struct afl::net::Server::StopState {
    afl::async::Operation operation;
};

/** State for "listen" operation. */
struct afl::net::Server::ListenerState {
    AcceptOperation operation;
};

/** State for handling a connection. */
struct afl::net::Server::ConnectionState {
    enum State {
        Idle,                   // Connection is idle/not doing anything.
        Sending,                // Connection is sending data. sendOperation and startTime are in use.
        Receiving,              // Connection is receiving data. receiveOperation, buffer, and startTime are in use.
        Closing                 // Connection is closing down.
    };
    State state;

    /** Socket. */
    afl::base::Ptr<Socket> socket;

    /** Name of socket.
        Stored upon construction to avoid allocation during runtime. */
    const String_t socketName;

    /** Protocol handler. */
    std::auto_ptr<ProtocolHandler> handler;

    /** Current operation. */
    ProtocolHandler::Operation phOperation;

    /** Current asynchronous send operation for state==Sending. */
    afl::async::SendOperation sendOperation;

    /** Current asynchronous receive operation for state==Receiving. */
    afl::async::ReceiveOperation receiveOperation;

    /** Timestamp of start of operation for state==Sending/Receiving. */
    uint32_t startTime;

    /** Buffer for receiving data. */
    uint8_t buffer[4096];

    ConnectionState(afl::base::Ptr<Socket> socket, const String_t& socketName)
        : state(Idle),
          socket(socket),
          socketName(socketName),
          handler(),
          phOperation(),
          sendOperation(),
          receiveOperation(),
          startTime(0)
        { }
};

// Constructor.
afl::net::Server::Server(afl::base::Ref<Listener> listener, ProtocolHandlerFactory& factory)
    : Stoppable(),
      m_listener(listener),
      m_factory(factory),
      m_stopSignal(0),
      m_log(),
      m_logName("net.server"),
      m_closeSignal(false)
{ }

// Destructor.
afl::net::Server::~Server()
{ }

// Entry point.
void
afl::net::Server::run()
{
    afl::async::Controller ctl;
    StopState stopState;
    ListenerState listenerState;
    afl::container::PtrVector<ConnectionState> connections;

    startStop(ctl, stopState);
    startListen(ctl, listenerState);

    /* Error mitigation: if the network stack goes into a state where accept immediately fails, we don't want to run in circles.
       Therefore, we count successive errors, and sleep upon a given limit to free the CPU. This will stall all active connections.
       However, since we expect this to happen only on terminal conditions (e.g. network stack is down or does not support accept),
       we expect this to not be a problem. */
    int errorCounter = 0;
    const int ERROR_LIMIT = 10;
    const int32_t ERROR_SLEEP = 3000;

    while (1) {
        afl::async::Operation* op = ctl.wait(findTimeout(connections, afl::sys::Time::getTickCounter()));
        if (op == 0) {
            // Timeout
            uint32_t now = afl::sys::Time::getTickCounter();
            for (size_t i = 0, n = connections.size(); i < n; ++i) {
                try {
                    handleConnectionTime(ctl, *connections[i], now);
                }
                catch (std::exception& e) {
                    logException(connections[i]->socketName, "timer", e.what());
                }
                catch (...) {
                    logException(connections[i]->socketName, "timer", UNKNOWN_EXCEPTION);
                }
            }
            errorCounter = 0;
        } else if (op == &listenerState.operation) {
            // New connection
            if (listenerState.operation.getResult().get() == 0) {
                // Error case: acceptAsync returned but did not create a socket?
                m_log.write(afl::sys::Log::Error, m_logName, "accept failed");

                ++errorCounter;
                if (errorCounter >= ERROR_LIMIT) {
                    afl::sys::Thread::sleep(ERROR_SLEEP);
                    errorCounter = 0;
                }
            } else {
                // Success case: create a new ProtocolHandler and ConnectionState
                String_t name = listenerState.operation.getResult()->getPeerName().toString();
                m_log.write(afl::sys::Log::Info, m_logName, afl::string::Format("%s: new connection", name));
                try {
                    std::auto_ptr<ConnectionState> newConnection(new ConnectionState(listenerState.operation.getResult(), name));
                    newConnection->handler.reset(m_factory.create());
                    startConnection(ctl, *connections.pushBackNew(newConnection.release()));
                }
                catch (std::exception& e) {
                    logException(name, "accept", e.what());
                }
                catch (...) {
                    logException(name, "accept", UNKNOWN_EXCEPTION);
                }
                errorCounter = 0;
            }

            // Wait for next connection
            startListen(ctl, listenerState);
        } else if (op == &stopState.operation) {
            // Stop
            break;
        } else {
            // Something else
            for (size_t i = 0, n = connections.size(); i < n; ++i) {
                try {
                    if (handleConnectionEvent(ctl, *connections[i], op)) {
                        break;
                    }
                }
                catch (std::exception& e) {
                    logException(connections[i]->socketName, "I/O", e.what());
                }
                catch (...) {
                    logException(connections[i]->socketName, "I/O", UNKNOWN_EXCEPTION);
                }
            }
            errorCounter = 0;
        }

        // Process closing
        if (m_closeSignal) {
            size_t out = 0;
            for (size_t in = 0, n = connections.size(); in < n; ++in) {
                if (connections[in]->state == ConnectionState::Closing) {
                    // Drop this connection
                    m_log.write(afl::sys::Log::Info, m_logName, afl::string::Format("%s: connection closes", connections[in]->socketName));
                    connections.replaceElementNew(in, 0);
                } else {
                    // Keep this connection
                    connections.swapElements(out++, in);
                }
            }
            connections.resize(out);
            m_closeSignal = false;
        }
    }

    // Terminate
    stopListen(ctl, listenerState);
    for (size_t i = 0, n = connections.size(); i < n; ++i) {
        stopConnection(ctl, *connections[i]);
    }
}

// Stop this server.
void
afl::net::Server::stop()
{
    m_stopSignal.post();
}

// Set name of log channel.
void
afl::net::Server::setLogName(const String_t& logName)
{
    m_logName = logName;
}

// Get log node.
afl::sys::Log&
afl::net::Server::log()
{
    return m_log;
}

/** Start reception of "stop" signal.
    \param ctl Controller
    \param state State */
void
afl::net::Server::startStop(afl::async::Controller& ctl, StopState& state)
{
    m_stopSignal.waitAsync(ctl, state.operation);
}

/** Start reception of "listen" signal, i.e. incoming connections.
    \param ctl Controller
    \param state State */
void
afl::net::Server::startListen(afl::async::Controller& ctl, ListenerState& state)
{
    state.operation.setResult(0);
    m_listener->acceptAsync(ctl, state.operation);
}

/** Start reception of signals on a connection, i.e. data transfers.
    \param ctl Controller
    \param state State */
void
afl::net::Server::startConnection(afl::async::Controller& ctl, ConnectionState& state)
{
    // Fetch new operation
    state.phOperation.m_dataToSend.reset();
    state.phOperation.m_close = false;
    state.phOperation.m_timeToWait = afl::sys::INFINITE_TIMEOUT;
    state.handler->getOperation(state.phOperation);

    if (!state.phOperation.m_dataToSend.empty()) {
        // Send
        state.sendOperation.setData(state.phOperation.m_dataToSend);
        state.socket->sendAsync(ctl, state.sendOperation);
        state.startTime = afl::sys::Time::getTickCounter();
        state.state = ConnectionState::Sending;
    } else if (state.phOperation.m_close) {
        // Close
        state.handler->handleConnectionClose();
        state.state = ConnectionState::Closing;
        m_closeSignal = true;
    } else {
        // Receive
        state.receiveOperation.setData(state.buffer);
        state.socket->receiveAsync(ctl, state.receiveOperation);
        state.startTime = afl::sys::Time::getTickCounter();
        state.state = ConnectionState::Receiving;
    }
}

/** Stop listening (ungraceful shutdown).
    \param ctl Controller
    \param state State */
void
afl::net::Server::stopListen(afl::async::Controller& ctl, ListenerState& state)
{
    m_listener->cancel(ctl, state.operation);
}

/** Stop a connection (ungraceful shutdown).
    \param ctl Controller
    \param state State */
void
afl::net::Server::stopConnection(afl::async::Controller& ctl, ConnectionState& state)
{
    switch (state.state) {
     case ConnectionState::Idle:
     case ConnectionState::Closing:
        break;

     case ConnectionState::Sending:
        state.socket->cancel(ctl, state.sendOperation);
        break;

     case ConnectionState::Receiving:
        state.socket->cancel(ctl, state.receiveOperation);
        break;
    }
}

/** Handle async event on a connection.
    Drives internal state machines and asks for next event, if required.
    \param ctl Controller
    \param state State
    \param op Incoming operation
    \retval true I handled this event
    \retval false I did not recognize this event */
bool
afl::net::Server::handleConnectionEvent(afl::async::Controller& ctl, ConnectionState& state, afl::async::Operation* op)
{
    switch (state.state) {
     case ConnectionState::Idle:
     case ConnectionState::Closing:
        return false;

     case ConnectionState::Sending:
        if (op == &state.sendOperation) {
            if (state.sendOperation.getUnsentBytes().empty()) {
                // Data sent completely
                state.handler->advanceTime(afl::sys::Time::getTickCounter() - state.startTime);
                state.state = ConnectionState::Idle;
                startConnection(ctl, state);
            } else {
                // Successful but incomplete send.
                // Remember sent bytes in phOperation.m_dataToSend so we can report that on timeout.
                state.phOperation.m_dataToSend = state.sendOperation.getUnsentBytes();
                state.sendOperation.setData(state.phOperation.m_dataToSend);
                state.socket->sendAsync(ctl, state.sendOperation);
                state.state = ConnectionState::Sending;
            }
            return true;
        } else {
            return false;
        }

     case ConnectionState::Receiving:
        if (op == &state.receiveOperation) {
            state.handler->advanceTime(afl::sys::Time::getTickCounter() - state.startTime);
            if (state.receiveOperation.getNumReceivedBytes() == 0) {
                // Received 0 bytes, i.e. other end closed connection
                state.handler->handleConnectionClose();
                state.state = ConnectionState::Closing;
                m_closeSignal = true;
            } else {
                // Process received data and fetch next command
                state.handler->handleData(state.receiveOperation.getReceivedBytes());
                state.state = ConnectionState::Idle;
                startConnection(ctl, state);
            }
            return true;
        } else {
            return false;
        }
    }
    return false;
}

/** Handle elapsed time.
    Determines whether this caused a timeout on a connection, and, if so, drives its state machine.
    \param ctl Controller
    \param state State
    \param now Current time */
void
afl::net::Server::handleConnectionTime(afl::async::Controller& ctl, ConnectionState& state, uint32_t now)
{
    uint32_t elapsed = now - state.startTime;
    uint32_t timeout = state.phOperation.m_timeToWait;
    switch (state.state) {
     case ConnectionState::Idle:
     case ConnectionState::Closing:
        break;

     case ConnectionState::Sending:
        if (timeout != afl::sys::INFINITE_TIMEOUT && elapsed >= timeout) {
            // Send timeout
            state.handler->advanceTime(elapsed);
            state.handler->handleSendTimeout(state.phOperation.m_dataToSend);
            state.socket->cancel(ctl, state.sendOperation);
            state.state = ConnectionState::Idle;
            startConnection(ctl, state);
        }
        break;

     case ConnectionState::Receiving:
        if (timeout != afl::sys::INFINITE_TIMEOUT && elapsed >= timeout) {
            // Receive timeout
            state.handler->advanceTime(elapsed);
            state.socket->cancel(ctl, state.receiveOperation);
            state.state = ConnectionState::Idle;
            startConnection(ctl, state);
        }
        break;
    }
}

/** Log exception.
    \param name Name of connection
    \param op Operation that was being performed
    \param what Exception text (what()) */
void
afl::net::Server::logException(const String_t& name, const char* op, const char* what)
{
    m_log.write(afl::sys::Log::Error, m_logName, afl::string::Format("%s: exception in %s: %s", name, op, what));
}

/** Find next timeout.
    \param connections Connection list
    \param now Current time */
uint32_t
afl::net::Server::findTimeout(const afl::container::PtrVector<ConnectionState>& connections, uint32_t now)
{
    uint32_t result = afl::sys::INFINITE_TIMEOUT;

    for (size_t i = 0, n = connections.size(); i < n; ++i) {
        const ConnectionState& state = *connections[i];
        switch (state.state) {
         case ConnectionState::Idle:
         case ConnectionState::Closing:
            break;

         case ConnectionState::Sending:
         case ConnectionState::Receiving:
            // Transfer with timeout: compute new timeout
            if (state.phOperation.m_timeToWait != afl::sys::INFINITE_TIMEOUT) {
                uint32_t elapsed = now - state.startTime;
                if (elapsed >= state.phOperation.m_timeToWait) {
                    result = 0;
                } else {
                    result = std::min(result, state.phOperation.m_timeToWait - elapsed);
                }
            }
            break;
        }
    }

    // Round up timeout
    if (result != 0 && result < MIN_TIME) {
        result = MIN_TIME;
    }

    return result;
}
