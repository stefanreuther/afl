/**
  *  \file afl/net/simpleserver.cpp
  *  \brief Class afl::net::SimpleServer
  */

#include <memory>
#include "afl/net/simpleserver.hpp"
#include "afl/net/listener.hpp"
#include "afl/net/protocolhandlerfactory.hpp"
#include "afl/net/protocolhandler.hpp"
#include "afl/net/socket.hpp"
#include "afl/net/interact.hpp"
#include "afl/sys/mutexguard.hpp"

afl::net::SimpleServer::SimpleServer(afl::base::Ref<Listener> listener, ProtocolHandlerFactory& factory)
    : afl::base::Stoppable(),
      m_listener(listener),
      m_factory(factory),
      m_mutex(),
      m_stopRequested(false)
{ }

afl::net::SimpleServer::~SimpleServer()
{ }

void
afl::net::SimpleServer::run()
{
    while (!isStopRequested()) {
        // Accept a connection
        afl::base::Ptr<Socket> socket = m_listener->accept();

        // Process it, protecting against exceptions on the way.
        try {
            // Make a handler
            std::auto_ptr<ProtocolHandler> handler(m_factory.create());

            // Operate.
            // If the factory fails to operate, just drop the connection.
            if (handler.get() != 0) {
                interact(*socket, *handler);
            }
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
afl::net::SimpleServer::stop()
{
    afl::sys::MutexGuard g(m_mutex);
    m_stopRequested = true;
}

inline bool
afl::net::SimpleServer::isStopRequested()
{
    afl::sys::MutexGuard g(m_mutex);
    return m_stopRequested;
}
