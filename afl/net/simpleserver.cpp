/**
  *  \file afl/net/simpleserver.cpp
  *  \brief Class afl::net::SimpleServer
  */

#include <memory>
#include "afl/net/simpleserver.hpp"
#include "afl/async/controller.hpp"
#include "afl/async/receiveoperation.hpp"
#include "afl/async/sendoperation.hpp"
#include "afl/net/listener.hpp"
#include "afl/net/protocolhandler.hpp"
#include "afl/net/protocolhandlerfactory.hpp"
#include "afl/net/socket.hpp"
#include "afl/sys/time.hpp"

namespace {
    /** Advance time for the listener.
        \param t Last time we've talked to the ProtocolHandler
        \param handler ProtocolHandler */
    void advanceTime(uint32_t& t, afl::net::ProtocolHandler& handler)
    {
        uint32_t t2 = afl::sys::Time::getTickCounter();
        handler.advanceTime(t2 - t);
        t = t2;
    }
}

afl::net::SimpleServer::SimpleServer(afl::base::Ptr<Listener> listener, ProtocolHandlerFactory& factory)
    : afl::base::Runnable(),
      m_listener(listener),
      m_factory(factory)
{ }

afl::net::SimpleServer::~SimpleServer()
{ }

void
afl::net::SimpleServer::run()
{
    while (1) {
        // Accept a connection
        afl::base::Ptr<Socket> socket = m_listener->accept();

        // Process it, protecting against exceptions on the way.
        try {
            handleConnection(*socket);
        }
        catch (std::exception& /*e*/) {
            // FIXME: log it
        }
        catch (...) {
            // FIXME: log it
        }
    }
}

void
afl::net::SimpleServer::handleConnection(Socket& socket)
{
    // Make a handler
    std::auto_ptr<ProtocolHandler> handler(m_factory.create());

    // Perform protocol operations
    uint32_t t = afl::sys::Time::getTickCounter();
    afl::async::Controller ctl;
    while (1) {
        // Get an operation
        ProtocolHandler::Operation op;
        op.m_dataToSend.reset();
        op.m_close = false;
        op.m_timeToWait = afl::sys::INFINITE_TIMEOUT;
        handler->getOperation(op);

        // Do it
        if (!op.m_dataToSend.empty()) {
            // Send requested
            while (1) {
                afl::async::SendOperation tx(op.m_dataToSend);
                if (!socket.send(ctl, tx, op.m_timeToWait)) {
                    // Send failed
                    advanceTime(t, *handler);
                    handler->handleSendTimeout(tx.getUnsentBytes());
                    break;
                } else {
                    // Send succeeded
                    op.m_dataToSend = tx.getUnsentBytes();
                    if (op.m_dataToSend.empty()) {
                        advanceTime(t, *handler);
                        break;
                    }
                }
            }
        } else if (op.m_close) {
            // Close requested
            handler->handleConnectionClose();
            break;
        } else {
            // Receive requested
            afl::async::ReceiveOperation rx(m_buffer);
            bool got = socket.receive(ctl, rx, op.m_timeToWait);

            // Update time
            advanceTime(t, *handler);

            // Submit data
            if (got) {
                if (rx.getNumReceivedBytes() == 0) {
                    // Other end closed
                    handler->handleConnectionClose();
                    break;
                } else {
                    handler->handleData(socket.getName(), rx.getReceivedBytes());
                }
            }
        }
    }
}
