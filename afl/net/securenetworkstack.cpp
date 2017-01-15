/**
  *  \file afl/net/securenetworkstack.cpp
  *  \brief Class afl::net::SecureNetworkStack
  */

#include "afl/net/securenetworkstack.hpp"
#include "afl/async/controller.hpp"
#include "afl/net/securesocket.hpp"
#include "afl/net/acceptoperation.hpp"
#include "afl/async/notifier.hpp"
#include "afl/sys/mutex.hpp"
#include "afl/sys/mutexguard.hpp"

/** Listener implementation for SecureNetworkStack.
    Wraps the real network stack's Listener to produce encrypted sockets.

    \todo The SSL handshake is performed synchronously even when we're doing asynchronous accept. */
class afl::net::SecureNetworkStack::Listener : public afl::net::Listener, private afl::async::Notifier {
 public:
    struct LocalAcceptOperation : public AcceptOperation {
        AcceptOperation* m_userOperation;
    };

    /** Constructor.
        Makes a new listener.
        \param listener Underlying listener
        \param ctx SecureContext that implements SSL */
    Listener(afl::base::Ref<afl::net::Listener> listener, afl::base::Ref<SecureContext> ctx);

    // Listener operations:
    virtual afl::base::Ptr<afl::net::Socket> accept(afl::sys::Timeout_t timeout);
    virtual void acceptAsync(afl::async::Controller& ctl, AcceptOperation& op);
    virtual void cancel(afl::async::Controller& ctl, afl::async::Operation& op);

 private:
    // Connections
    const afl::base::Ref<afl::net::Listener> m_listener;      ///< Underlying listener.
    const afl::base::Ref<afl::net::SecureContext> m_ctx;      ///< SecureContext that implements SSL.

    // Async:
    std::list<LocalAcceptOperation*> m_pendingAccepts;       ///< All pending encrypted accept operations.
    afl::sys::Mutex m_mutex;                                 ///< Mutex for m_pendingAccepts

    // Notifier operations:
    virtual void notify(afl::async::Operation& op);
    virtual void notifyDirect(afl::async::Operation& op);
};

inline
afl::net::SecureNetworkStack::Listener::Listener(afl::base::Ref<afl::net::Listener> listener, afl::base::Ref<SecureContext> ctx)
    : m_listener(listener),
      m_ctx(ctx),
      m_pendingAccepts()
{ }

// Implementation of Listener::accept
afl::base::Ptr<afl::net::Socket>
afl::net::SecureNetworkStack::Listener::accept(afl::sys::Timeout_t timeout)
{
    // Accept socket on physical layer
    afl::base::Ptr<afl::net::Socket> sock = m_listener->accept(timeout);
    if (sock.get() != 0) {
        // Wrap in SSL
        afl::async::Controller ctl;
        return m_ctx->wrapServer(ctl, *sock).asPtr();
    } else {
        return sock;
    }
}

// Implementation of Listener::acceptAsync
void
afl::net::SecureNetworkStack::Listener::acceptAsync(afl::async::Controller& ctl, AcceptOperation& op)
{
    // Configure user operation
    op.setController(&ctl);

    // Make our own accept operation
    LocalAcceptOperation* localOperation = new LocalAcceptOperation();
    localOperation->m_userOperation = &op;
    localOperation->setNotifier(*this);

    // Start it and remember in local list.
    {
        afl::sys::MutexGuard g(m_mutex);
        m_pendingAccepts.push_back(localOperation);
    }
    m_listener->acceptAsync(ctl, *localOperation);
}

// Implementation of Cancelable::cancel
void
afl::net::SecureNetworkStack::Listener::cancel(afl::async::Controller& ctl, afl::async::Operation& op)
{
    // Cancel in local list
    {
        afl::sys::MutexGuard g(m_mutex);
        for (std::list<LocalAcceptOperation*>::iterator it = m_pendingAccepts.begin(), e = m_pendingAccepts.end(); it != e; ++it) {
            LocalAcceptOperation* localOperation = *it;
            if (localOperation->m_userOperation == &op) {
                // Remove from local list
                m_pendingAccepts.erase(it);

                // Cancel on underlying network stack
                m_listener->cancel(ctl, *localOperation->m_userOperation);
                delete localOperation;
                break;
            }
        }
    }

    // Cancel in controller
    ctl.revertPost(op);
}

// Implementation of Notifier::notify
void
afl::net::SecureNetworkStack::Listener::notify(afl::async::Operation& op)
{
    LocalAcceptOperation& localOperation = static_cast<LocalAcceptOperation&>(op);

    // Remove from list
    {
        afl::sys::MutexGuard g(m_mutex);
        m_pendingAccepts.remove(&localOperation);
    }

    // Update
    AcceptOperation& userOperation = *localOperation.m_userOperation;
    afl::async::Controller& ctl = *userOperation.getController();
    try {
        afl::base::Ref<Socket> result = m_ctx->wrapServer(ctl, *localOperation.getResult());
        userOperation.setResult(result.asPtr());
        userOperation.getNotifier().notify(userOperation);
    }
    catch (...) {
        // Operation fails; retry.
        {
            afl::sys::MutexGuard g(m_mutex);
            m_pendingAccepts.push_back(&localOperation);
        }
        m_listener->acceptAsync(ctl, localOperation);
    }
}

void
afl::net::SecureNetworkStack::Listener::notifyDirect(afl::async::Operation& op)
{
    notify(op);
}

/*************************** SecureNetworkStack **************************/

afl::net::SecureNetworkStack::SecureNetworkStack(NetworkStack& peer, afl::base::Ref<SecureContext> ctx)
    : NetworkStack(),
      m_peer(peer),
      m_ctx(ctx)
{ }

afl::net::SecureNetworkStack::~SecureNetworkStack()
{ }

afl::base::Ref<afl::net::Listener>
afl::net::SecureNetworkStack::listen(const Name& name, int backlogSize)
{
    return *new Listener(m_peer.listen(name, backlogSize), m_ctx);
}

afl::base::Ref<afl::net::Socket>
afl::net::SecureNetworkStack::connect(const Name& name, afl::sys::Timeout_t timeout)
{
    afl::async::Controller ctl;
    return m_ctx->wrapClient(ctl, m_peer.connect(name, timeout));
}
