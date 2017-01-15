/**
  *  \file afl/net/http/defaultconnectionprovider.cpp
  *  \brief Class afl::net::http::DefaultConnectionProvider
  */

#include "afl/net/http/defaultconnectionprovider.hpp"
#include "afl/sys/mutexguard.hpp"
#include "afl/net/name.hpp"
#include "afl/net/http/client.hpp"
#include "afl/net/http/clientconnection.hpp"
#include "afl/string/messages.hpp"
#include "afl/net/http/clientrequest.hpp"

namespace {
    const uint32_t CONNECTION_TIMEOUT = 30000;
}

afl::net::http::DefaultConnectionProvider::DefaultConnectionProvider(Client& client, NetworkStack& stack, String_t scheme)
    : m_client(client),
      m_networkStack(stack),
      m_scheme(scheme),
      m_thread("HTTP Connection Provider", *this),
      m_wake(0),
      m_mutex(),
      m_stop(false)
{
    m_thread.start();
}

afl::net::http::DefaultConnectionProvider::~DefaultConnectionProvider()
{
    // Tell thread to stop
    {
        afl::sys::MutexGuard g(m_mutex);
        m_stop = true;
    }
    m_wake.post();

    // Stop it
    m_thread.join();
}

void
afl::net::http::DefaultConnectionProvider::requestNewConnection()
{
    m_wake.post();
}

void
afl::net::http::DefaultConnectionProvider::run()
{
    while (1) {
        // Wait for something to happen
        m_wake.wait();

        // Stop requested?
        {
            afl::sys::MutexGuard g(m_mutex);
            if (m_stop) {
                break;
            }
        }

        // Create requested connections
        Name name;
        String_t scheme;
        while (m_client.getUnsatisfiedTarget(name, scheme)) {
            if (scheme != m_scheme) {
                // Mismatching scheme, request cannot be fulfilled
                m_client.cancelRequestsByTarget(name, scheme,
                                                http::ClientRequest::UnsupportedProtocol,
                                                afl::string::Messages::invalidUrl());
            } else {
                try {
                    // Try connecting...
                    afl::base::Ref<afl::net::Socket> socket = m_networkStack.connect(name, CONNECTION_TIMEOUT);
                    m_client.addNewConnection(new ClientConnection(name, scheme, socket));
                }
                catch (std::exception& e) {
                    // Regular failure case
                    m_client.cancelRequestsByTarget(name, scheme,
                                                    ClientRequest::ConnectionFailed,
                                                    e.what());
                }
                catch (...) {
                    // Irregular failure case; avoid that exceptions kill the thread
                    m_client.cancelRequestsByTarget(name, scheme,
                                                    ClientRequest::ConnectionFailed,
                                                    afl::string::Messages::unknownError());
                }
            }
        }
    }
}
