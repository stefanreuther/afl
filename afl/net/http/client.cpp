/**
  *  \file afl/net/http/client.cpp
  *  \brief Class afl::net::http::Client
  */

#include <cassert>
#include <memory>
#include "afl/net/http/client.hpp"
#include "afl/async/controller.hpp"
#include "afl/net/http/clientconnection.hpp"
#include "afl/net/http/clientconnectionprovider.hpp"
#include "afl/net/http/clientrequest.hpp"
#include "afl/sys/mutexguard.hpp"
#include "afl/string/messages.hpp"
#include "afl/sys/time.hpp"

namespace {
    /** Wait interval. This is the time base for computing timeouts. */
    const uint32_t WAIT_INTERVAL = 5000;
}

afl::net::http::Client::Client()
    : afl::base::Stoppable(),
      m_provider(),
      m_mutex(),
      m_wake(0),
      m_connections(),
      m_requests(),
      m_cancels(),
      m_stop(false),
      m_needNewConnection(false),
      m_requestIdCounter(0)
{ }

afl::net::http::Client::~Client()
{
    // Stop it
    if (!m_stop) {
        stop();
    }

    // Now kill the connection provider.
    // Sequencing is pretty critical here.
    // A typical CCP might call back into us when we want it to die.
    // We must therefore destroy it without our lock held (but m_provider is protected by the mutex).
    // We must destroy it before destroying our other innards
    // so it does not call back into us when half of our members are already gone (m_mutex!).
    std::auto_ptr<ClientConnectionProvider> provider;
    {
        afl::sys::MutexGuard g(m_mutex);
        provider = m_provider;
    }
    provider.reset();
}

void
afl::net::http::Client::addNewConnection(ClientConnection* conn)
{
    assert(conn != 0);
    {
        afl::sys::MutexGuard g(m_mutex);
        m_connections.pushBackNew(conn);
    }
    m_wake.post();
}

uint32_t
afl::net::http::Client::addNewRequest(ClientRequest* req)
{
    uint32_t id;
    assert(req != 0);
    {
        afl::sys::MutexGuard g(m_mutex);
        id = ++m_requestIdCounter;
        req->setRequestId(id);
        if (m_stop) {
            // This client is already stopped, the thread is no longer responding.
            // Fail the request immediately.
            std::auto_ptr<ClientRequest> r(req);
            r->handleFailure(ClientRequest::Cancelled, afl::string::Messages::operationCancelled());
        } else {
            // Post into thread.
            m_requests.pushBackNew(req);
            m_needNewConnection = true;
        }
    }
    m_wake.post();
    return id;
}

void
afl::net::http::Client::cancelRequest(uint32_t id)
{
    afl::sys::MutexGuard g(m_mutex);

    // If it's in m_requests, cancel it from there
    bool found = false;
    for (size_t i = 0, n = m_requests.size(); i < n; ++i) {
        if (m_requests[i]->getRequestId() == id) {
            m_requests[i]->handleFailure(ClientRequest::Cancelled, afl::string::Messages::operationCancelled());
            m_requests.erase(m_requests.begin() + i);
            found = true;
            break;
        }
    }

    // If it's not in m_requests, it might be in a connection.
    // Let the worker do it.
    if (!found) {
        m_cancels.push_back(id);
        m_wake.post();
    }
}

void
afl::net::http::Client::setNewConnectionProvider(ClientConnectionProvider* provider)
{
    assert(provider != 0);
    afl::sys::MutexGuard g(m_mutex);
    m_provider.reset(provider);
}

void
afl::net::http::Client::run()
{
    // Wakeup and integration
    afl::async::Controller ctl;
    afl::async::Operation wakeOp;
    bool wakeActive = false;
    uint32_t time = afl::sys::Time::getTickCounter();

    while (1) {
        // Notify listeners
        requestNewConnection();

        // When not waiting for wakeup, do so
        if (!wakeActive) {
            m_wake.waitAsync(ctl, wakeOp);
            wakeActive = true;
        }

        // Deal with new connections
        // FIXME: optimize by not processing ALL connections
        processConnections(0, ctl, 0, 0);

        // Wait for something to happen
        afl::async::Operation* op = ctl.wait(WAIT_INTERVAL);
        if (op == &wakeOp) {
            // External function call
            if (m_stop) {
                break;
            }
            processCancels(ctl);
            wakeActive = false;
        } else {
            // No external call. Handle with internal
            uint32_t now = afl::sys::Time::getTickCounter();
            uint32_t elapsed = now - time;
            time = now;
            processConnections(0, ctl, op, elapsed);
        }
    }

    // Cancel everything
    {
        afl::sys::MutexGuard g(m_mutex);

        // Cancel pending requests
        for (size_t i = 0; i < m_requests.size(); ++i) {
            std::auto_ptr<ClientRequest> req(m_requests.extractElement(i));
            if (req.get() != 0) {
                req->handleFailure(ClientRequest::Cancelled, afl::string::Messages::operationCancelled());
            }
        }

        // Cancel requests currently being worked on
        for (size_t i = 0; i < m_connections.size(); ++i) {
            m_connections[i]->cancel(ctl);
        }
    }
}

void
afl::net::http::Client::stop()
{
    {
        afl::sys::MutexGuard g(m_mutex);
        m_stop = true;
    }
    m_wake.post();
}

void
afl::net::http::Client::cancelRequestsByTarget(const Name& name,
                                               const String_t& scheme,
                                               ClientRequest::FailureReason reason,
                                               const String_t& message)
{
    afl::sys::MutexGuard g(m_mutex);
    for (size_t reqIndex = 0; reqIndex < m_requests.size(); /* no increment */) {
        if (m_requests[reqIndex]->getName() == name && m_requests[reqIndex]->getScheme() == scheme) {
            m_requests[reqIndex]->handleFailure(reason, message);
            m_requests.erase(m_requests.begin() + reqIndex);
        } else {
            ++reqIndex;
        }
    }
}

bool
afl::net::http::Client::getUnsatisfiedTarget(Name& name, String_t& scheme)
{
    afl::sys::MutexGuard g(m_mutex);
    for (size_t reqIndex = 0, numReqs = m_requests.size(); reqIndex < numReqs; ++reqIndex) {
        bool found = false;
        for (size_t connIndex = 0, numConns = m_connections.size(); connIndex < numConns; ++connIndex) {
            if (m_connections[connIndex]->matchRequest(*m_requests[reqIndex])) {
                found = true;
                break;
            }
        }
        if (!found) {
            name   = m_requests[reqIndex]->getName();
            scheme = m_requests[reqIndex]->getScheme();
            return true;
        }
    }
    return false;
}

void
afl::net::http::Client::requestNewConnection()
{
    afl::sys::MutexGuard g(m_mutex);
    if (m_needNewConnection) {
        m_needNewConnection = false;
        if (m_provider.get() != 0) {
            m_provider->requestNewConnection();
        }
    }
}

void
afl::net::http::Client::processConnections(size_t i, afl::async::Controller& ctl, afl::async::Operation* op, uint32_t elapsed)
{
    afl::sys::MutexGuard g(m_mutex);
    while (i < m_connections.size()) {
        ClientConnection& conn = *m_connections[i];
        switch (conn.handleEvent(ctl, op, elapsed)) {
         case ClientConnection::WaitForRequest:
            // Do we have a matching request?
            if (ClientRequest* req = extractMatchingRequest(conn)) {
                // Great! Set it and repeat loop.
                conn.setNewRequest(req);
            } else {
                // No request; advance with next slot.
                ++i;
            }
            break;

         case ClientConnection::Transferring:
            // Nothing to do, go to next slot
            ++i;
            break;

         case ClientConnection::Shutdown:
            // This connection wants to close.
            // Save the request, if any.
            if (ClientRequest* req = conn.extractRequest()) {
                m_requests.insertNew(m_requests.begin(), req);
            }

            // We may need to fetch a new connection
            m_needNewConnection = true;

            // Delete this connection
            m_connections.erase(m_connections.begin()+i);
            break;
        }
    }
}

afl::net::http::ClientRequest*
afl::net::http::Client::extractMatchingRequest(ClientConnection& conn)
{
    afl::sys::MutexGuard g(m_mutex);
    ClientRequest* result = 0;
    for (size_t i = 0; i < m_requests.size(); ++i) {
        if (conn.matchRequest(*m_requests[i])) {
            result = m_requests.extractElement(i);
            m_requests.erase(m_requests.begin() + i);
            break;
        }
    }
    return result;
}

void
afl::net::http::Client::processCancels(afl::async::Controller& ctl)
{
    afl::sys::MutexGuard g(m_mutex);
    if (!m_cancels.empty()) {
        size_t i = 0;
        while (i < m_connections.size()) {
            ClientConnection& conn = *m_connections[i];
            for (size_t ci = 0, cn = m_cancels.size(); ci < cn; ++ci) {
                conn.cancelRequest(ctl, m_cancels[ci]);
            }
            if (conn.handleEvent(ctl, 0, 0) == ClientConnection::Shutdown) {
                // Connection wants to close.
                assert(conn.extractRequest() == 0);
                m_connections.erase(m_connections.begin()+i);
            } else {
                // Keep the connection
                ++i;
            }
        }
        m_cancels.clear();
    }
}
