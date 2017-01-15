/**
  *  \file config/openssl/opensslsocket.cpp
  *  \brief Class config::openssl::OpenSSLSocket
  */

#include <cassert>
#include "afl/config.h"

#ifdef HAVE_OPENSSL
# include "config/openssl/opensslsocket.hpp"
# include "config/openssl/opensslexception.hpp"
# include "afl/net/name.hpp"
# include "afl/sys/mutexguard.hpp"
# include "config/openssl/opensslcontext.hpp"

# ifndef ERR_R_INTERNAL_ERROR
#  define ERR_R_INTERNAL_ERROR ERR_R_FATAL
# endif


config::openssl::OpenSSLSocket::OpenSSLSocket(afl::base::Ref<OpenSSLContext> ctx, afl::base::Ref<Socket> peer)
    : m_peer(peer),
      m_mutex(),
      m_ssl(0),
      m_sslEnd(0),
      m_myEnd(0),
      m_actions(),
      m_state(Idle),
      m_connectStatus(false),
      m_connectError(0)
{
    // Make a BIO for transferring data.
    // Giving it 4k buffer should be plenty of room so that BIO_write never fails.
    const size_t BIO_SIZE = 4*sizeof(m_buffer);
    if (!BIO_new_bio_pair(&m_sslEnd, BIO_SIZE, &m_myEnd, BIO_SIZE)) {
        throw OpenSSLException(ERR_get_error());
    }

    m_ssl = ctx->createSSL();
    if (m_ssl == 0) {
        throw OpenSSLException(ERR_get_error());
    }
    SSL_set_bio(m_ssl, m_sslEnd, m_sslEnd);
}

config::openssl::OpenSSLSocket::~OpenSSLSocket()
{
    SSL_free(m_ssl);
    BIO_free(m_myEnd);
}

// Socket:
void
config::openssl::OpenSSLSocket::closeSend()
{
    // We could do a SSL_shutdown here.
    // The problem is that at this point we have no Controller.
}

afl::net::Name
config::openssl::OpenSSLSocket::getPeerName()
{
    return m_peer->getPeerName();
}

// CommunicationObject:
bool
config::openssl::OpenSSLSocket::send(afl::async::Controller& ctl, afl::async::SendOperation& op, afl::sys::Timeout_t timeout)
{
    sendAsync(ctl, op);
    if (!ctl.wait(op, timeout)) {
        cancel(ctl, op);
        return false;
    } else {
        return true;
    }
}

void
config::openssl::OpenSSLSocket::sendAsync(afl::async::Controller& ctl, afl::async::SendOperation& op)
{
    afl::sys::MutexGuard g(m_mutex);
    op.setController(&ctl);
    m_actions.push_back(Action(Action::Send, &op));
    cycle();
}

bool
config::openssl::OpenSSLSocket::receive(afl::async::Controller& ctl, afl::async::ReceiveOperation& op, afl::sys::Timeout_t timeout)
{
    receiveAsync(ctl, op);
    if (!ctl.wait(op, timeout)) {
        cancel(ctl, op);
        return false;
    } else {
        return true;
    }
}

void
config::openssl::OpenSSLSocket::receiveAsync(afl::async::Controller& ctl, afl::async::ReceiveOperation& op)
{
    afl::sys::MutexGuard g(m_mutex);
    op.setController(&ctl);
    m_actions.push_back(Action(Action::Receive, &op));
    cycle();
}

void
config::openssl::OpenSSLSocket::cancel(afl::async::Controller& ctl, afl::async::Operation& op)
{
    afl::sys::MutexGuard g(m_mutex);
    if (!m_actions.empty() && &op == m_actions.front().m_pOperation) {
        // We're canceling the front operation
        m_actions.pop_front();
        switch (m_state) {
         case Idle:
         case Failed:
         case Closed:
            // Nothing to do.
            break;

         case Receiving:
            m_peer->cancel(ctl, m_receiveOperation);
            m_state = Failed;
            cycle();
            break;

         case Sending:
            m_peer->cancel(ctl, m_sendOperation);
            m_state = Failed;
            cycle();
            break;
        }
    } else {
        // We're canceling something else
        for (std::list<Action>::iterator it = m_actions.begin(); it != m_actions.end(); ++it) {
            if (it->m_pOperation == &op) {
                m_actions.erase(it);
                break;
            }
        }
        ctl.revertPost(op);
    }
}

String_t
config::openssl::OpenSSLSocket::getName()
{
    return m_peer->getName();
}

void
config::openssl::OpenSSLSocket::connect(afl::async::Controller& ctl)
{
    afl::async::Operation op;
    op.setController(&ctl);
    m_actions.push_back(Action(Action::Connect, &op));
    cycle();
    ctl.wait(op);
    if (!m_connectStatus) {
        throw OpenSSLException(m_connectError);
    }
}

void
config::openssl::OpenSSLSocket::accept(afl::async::Controller& ctl)
{
    afl::async::Operation op;
    op.setController(&ctl);
    m_actions.push_back(Action(Action::Accept, &op));
    cycle();
    ctl.wait(op);
    if (!m_connectStatus) {
        throw OpenSSLException(m_connectError);
    }
}

void
config::openssl::OpenSSLSocket::notify(afl::async::Operation& op)
{
    afl::sys::MutexGuard g(m_mutex);
    notifyDirect(op);
}

void
config::openssl::OpenSSLSocket::notifyDirect(afl::async::Operation& op)
{
    switch (m_state) {
     case Idle:
     case Failed:
     case Closed:
        assert(0);
        break;

     case Sending:
        assert(&op == &m_sendOperation);
        assert(!m_actions.empty());
        if (m_sendOperation.getNumSentBytes() == 0) {
            // I could not send anything.
            // This means our connection is broken.
            m_state = Failed;
        } else if (m_sendOperation.getUnsentBytes().size() == 0) {
            // I could send everything. Check whether we want to send more.
            int n = BIO_read(m_myEnd, m_buffer, sizeof(m_buffer));
            if (n > 0) {
                m_state = Sending;
                m_sendOperation.setData(afl::base::ConstBytes_t::unsafeCreate(m_buffer, n));
                m_sendOperation.setNotifier(*this);
                m_peer->sendAsync(*m_actions.front().m_pOperation->getController(), m_sendOperation);
            } else {
                m_state = Idle;
            }
        } else {
            // Partial send. Attempt to send the remainder.
            m_state = Sending;
            m_sendOperation.setData(m_sendOperation.getUnsentBytes());
            m_sendOperation.setNotifier(*this);
            m_peer->sendAsync(*m_actions.front().m_pOperation->getController(), m_sendOperation);
        }
        cycle();
        break;

     case Receiving:
        assert(&op == &m_receiveOperation);
        if (m_receiveOperation.getReceivedBytes().size() == 0) {
            // I wanted to receive, but could not. This means the connection was closed.
            m_state = Closed;
        } else {
            // OK, got some bytes. Feed them into the BIO.
            int n = BIO_write(m_myEnd, m_receiveOperation.getReceivedBytes().unsafeData(),
                              Size_t(m_receiveOperation.getReceivedBytes().size()));
            m_state = Idle;

            // Because OpenSSL wanted us to read, and our BIO buffer is larger than our read buffer, it must fit.
            assert(n == int(m_receiveOperation.getReceivedBytes().size()));
            (void) n;
        }
        cycle();
        break;
    }
}

config::openssl::OpenSSLSocket::ResultClass
config::openssl::OpenSSLSocket::classifyResult(int result)
{
    int n = SSL_get_error(m_ssl, result);
    switch (n) {
     case SSL_ERROR_NONE:
        return Success;

     case SSL_ERROR_WANT_WRITE:
        // flush buffer
        n = BIO_read(m_myEnd, m_buffer, sizeof(m_buffer));
        if (n > 0) {
            m_state = Sending;
            m_sendOperation.setData(afl::base::ConstBytes_t::unsafeCreate(m_buffer, n));
            m_sendOperation.setNotifier(*this);
            m_peer->sendAsync(*m_actions.front().m_pOperation->getController(), m_sendOperation);
            return Wait;
        } else {
            // OpenSSL told us to send, but doesn't have data to send? That's a failure.
            m_state = Failed;
            return Failure;
        }

     case SSL_ERROR_WANT_READ:
        // flush buffer before trying to read
        n = BIO_read(m_myEnd, m_buffer, sizeof(m_buffer));
        if (n > 0) {
            m_state = Sending;
            m_sendOperation.setData(afl::base::ConstBytes_t::unsafeCreate(m_buffer, n));
            m_sendOperation.setNotifier(*this);
            m_peer->sendAsync(*m_actions.front().m_pOperation->getController(), m_sendOperation);
        } else {
            m_state = Receiving;
            m_receiveOperation.setData(m_buffer);
            m_receiveOperation.setNotifier(*this);
            m_peer->receiveAsync(*m_actions.front().m_pOperation->getController(), m_receiveOperation);
        }
        return Wait;

     case SSL_ERROR_SSL:
        // ERR_print_errors_fp(stdout);
        return Failure;

     default:
        return Failure;
    }
}

void
config::openssl::OpenSSLSocket::cycle()
{
    while ((m_state != Receiving && m_state != Sending) && !m_actions.empty()) {
        // Check current action
        Action& action = m_actions.front();
        bool finished = true;
        switch (action.m_kind) {
         case Action::Connect:
            finished = cycleConnect(action);
            break;

         case Action::Accept:
            finished = cycleAccept(action);
            break;

         case Action::Send:
            finished = cycleSend(action);
            break;

         case Action::Receive:
            finished = cycleReceive(action);
            break;

         case Action::Flush:
            finished = cycleFlush();
            break;
        }

        // If action finished, notify user.
        if (finished) {
            action.m_pOperation->getNotifier().notify(*action.m_pOperation);
            m_actions.pop_front();
        }
    }
}

inline bool
config::openssl::OpenSSLSocket::cycleConnect(Action& a)
{
    switch (m_state) {
     case Idle:
        switch (classifyResult(SSL_connect(m_ssl))) {
         case Success:
            m_connectStatus = true;
            a.m_kind = Action::Flush;
            break;

         case Failure:
            m_connectStatus = false;
            m_connectError = ERR_get_error();
            return true;

         case Wait:
            break;
        }
        break;

     case Sending:
     case Receiving:
        break;

     case Failed:
     case Closed:
        m_connectStatus = false;
        m_connectError = ERR_R_INTERNAL_ERROR;
        return true;
    }

    return false;
}

inline bool
config::openssl::OpenSSLSocket::cycleAccept(Action& a)
{
    switch (m_state) {
     case Idle:
        int zz;
        zz = SSL_accept(m_ssl);
        switch (classifyResult(zz)) {
         case Success:
            m_connectStatus = true;
            a.m_kind = Action::Flush;
            break;

         case Failure:
            m_connectStatus = false;
            m_connectError = ERR_get_error();
            return true;

         case Wait:
            break;
        }
        break;

     case Sending:
     case Receiving:
        break;

     case Failed:
     case Closed:
        m_connectStatus = false;
        m_connectError = ERR_R_INTERNAL_ERROR;
        return true;
    }

    return false;
}

inline bool
config::openssl::OpenSSLSocket::cycleSend(Action& a)
{
    afl::async::SendOperation* sendOperation = static_cast<afl::async::SendOperation*>(a.m_pOperation);;
    assert(sendOperation != 0);

    switch (m_state) {
     case Idle:
        if (!sendOperation->isCompleted()) {
            // We want to send
            int result = SSL_write(m_ssl, sendOperation->getUnsentBytes().unsafeData(),
                                   Size_t(sendOperation->getUnsentBytes().size()));
            switch (classifyResult(result)) {
             case Success:
                if (result > 0) {
                    // We sent a nonzero amount of data. Add it and retry.
                    sendOperation->addSentBytes(result);
                    return false;
                } else {
                    // We sent nothing and have no indication that retrying will change that.
                    return true;
                }
                break;
             case Failure:
                return true;
             case Wait:
                break;
            }
        } else {
            // Operation is already complete, flush out pending data.
            a.m_kind = Action::Flush;
        }
        break;

     case Sending:
     case Receiving:
        break;

     case Failed:
     case Closed:
        return true;
    }

    return false;
}

inline bool
config::openssl::OpenSSLSocket::cycleReceive(Action& a)
{
    afl::async::ReceiveOperation* receiveOperation = static_cast<afl::async::ReceiveOperation*>(a.m_pOperation);
    assert(receiveOperation != 0);
    switch (m_state) {
     case Idle:
        if (!receiveOperation->isCompleted() && receiveOperation->getNumReceivedBytes() == 0) {
            // We want to receive
            int result = SSL_read(m_ssl, receiveOperation->getUnreceivedBytes().unsafeData(),
                                  Size_t(receiveOperation->getUnreceivedBytes().size()));
            switch (classifyResult(result)) {
             case Success:
                if (result > 0) {
                    // OK, got some bytes; retry.
                    receiveOperation->addReceivedBytes(result);
                    return false;
                } else {
                    // We got nothing and have no indication that retrying will change that.
                    return true;
                }
             case Failure:
                return true;
             case Wait:
                break;
            }
        } else {
            // Operation is already (partially) complete, notify it.
            a.m_kind = Action::Flush;
        }
        break;

     case Sending:
     case Receiving:
        break;

     case Failed:
     case Closed:
        return true;
    }

    return false;
}

inline bool
config::openssl::OpenSSLSocket::cycleFlush()
{
    int n = BIO_read(m_myEnd, m_buffer, sizeof(m_buffer));
    if (n > 0) {
        m_state = Sending;
        m_sendOperation.setData(afl::base::ConstBytes_t::unsafeCreate(m_buffer, n));
        m_sendOperation.setNotifier(*this);
        m_peer->sendAsync(*m_actions.front().m_pOperation->getController(), m_sendOperation);
        return false;
    } else {
        return true;
    }
}

#endif
