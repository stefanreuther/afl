/**
  *  \file afl/net/internalnetworkstack.cpp
  *  \brief Class afl::net::InternalNetworkStack
  */

#include "afl/net/internalnetworkstack.hpp"

#include "afl/async/controller.hpp"
#include "afl/async/receiveoperation.hpp"
#include "afl/async/sendoperation.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/net/acceptoperation.hpp"
#include "afl/sys/mutexguard.hpp"

using afl::async::Controller;
using afl::async::Operation;
using afl::async::ReceiveOperation;
using afl::async::SendOperation;
using afl::base::Ptr;
using afl::base::Ref;
using afl::except::FileProblemException;
using afl::sys::MutexGuard;

namespace {
    template<typename T, typename O>
    void removeElement(std::vector<T*>& vec, O& ele)
    {
        typename std::vector<T*>::iterator it = std::find(vec.begin(), vec.end(), &ele);
        if (it != vec.end()) {
            vec.erase(it);
        }
    }
}

/*
 *  Stream: a single stream transfer with (sort-of) socket semantics
 *
 *  Collect senders and receivers, and if we have a pair, try to move data between them.
 *  Between method calls, only one of the two lists can be nonempty, because otherwise,
 *  we would have made a pair.
 *
 *  Stream is the final object that does not own any other objects.
 */

class afl::net::InternalNetworkStack::Stream : public afl::base::RefCounted {
 public:
    Stream(const Name& name);
    const Name& getName() const;
    void sendAsync(Controller& ctl, SendOperation& op);
    void receiveAsync(Controller& ctl, ReceiveOperation& op);
    void cancel(Controller& ctl, Operation& op);
    void closeSend();

 private:
    const Name m_name;
    afl::sys::Mutex m_mutex;
    std::vector<SendOperation*> m_sends;
    std::vector<ReceiveOperation*> m_receives;
    bool m_sendClosed;

    void tryMove();
};

inline
afl::net::InternalNetworkStack::Stream::Stream(const Name& name)
    : m_name(name),
      m_mutex(),
      m_sends(),
      m_receives(),
      m_sendClosed(false)
{ }

inline const afl::net::Name&
afl::net::InternalNetworkStack::Stream::getName() const
{
    return m_name;
}

void
afl::net::InternalNetworkStack::Stream::sendAsync(Controller& ctl, SendOperation& op)
{
    MutexGuard g(m_mutex);
    op.setController(&ctl);
    m_sends.push_back(&op);
    tryMove();
}

void
afl::net::InternalNetworkStack::Stream::receiveAsync(Controller& ctl, ReceiveOperation& op)
{
    MutexGuard g(m_mutex);
    op.setController(&ctl);
    m_receives.push_back(&op);
    tryMove();
}

void
afl::net::InternalNetworkStack::Stream::cancel(Controller& ctl, Operation& op)
{
    MutexGuard g(m_mutex);
    removeElement(m_sends, op);
    removeElement(m_receives, op);
    ctl.revertPost(op);
}

inline void
afl::net::InternalNetworkStack::Stream::closeSend()
{
    MutexGuard g(m_mutex);
    m_sendClosed = true;
    tryMove();
}

void
afl::net::InternalNetworkStack::Stream::tryMove()
{
    while (!m_receives.empty()) {
        // Got a receive, try to satisfy it with sends, finishing the sends when complete.
        ReceiveOperation* recv = m_receives.front();
        while (!recv->isCompleted() && !m_sends.empty()) {
            SendOperation* send = m_sends.front();
            recv->copyFrom(*send);
            if (send->isCompleted()) {
                m_sends.erase(m_sends.begin());
                send->getController()->post(*send);
            }
        }

        // If we received anything, or the send end closed, report this receive ready.
        // Sockets can do partial receive.
        if (recv->getNumReceivedBytes() > 0 || m_sendClosed) {
            m_receives.erase(m_receives.begin());
            recv->getController()->post(*recv);
        } else {
            // This receive still didn't get anything, there is no send,
            // but there might be one in the future (not m_sendClosed): wait.
            break;
        }
    }
}


/*
 *  InternalSocket: implementation of Socket
 *
 *  A socket pair created by pairing a connect/accept provides two data streams, for both flow directions.
 *  A connection therefore has two InternalSocket objects, connected crosswise to two Stream objects.
 *
 *    ,----------------. -------> ,--------.
 *    | InternalSocket |          | Stream |
 *    `----------------' --. ,--> `--------'
 *                          X
 *    ,----------------. --' `--> ,--------.
 *    | InternalSocket |          | Stream |
 *    ´----------------' -------> `--------'
 *
 *  Stream objects are not visible to the outside; they live as long as at least one InternalSocket is alive.
 */

class afl::net::InternalNetworkStack::InternalSocket : public afl::net::Socket {
 public:
    InternalSocket(const Ref<Stream>& fromMe, const Ref<Stream>& fromThem)
        : m_fromMe(fromMe),
          m_fromThem(fromThem)
        { }
    ~InternalSocket()
        { m_fromMe->closeSend(); }
    virtual bool send(Controller& ctl, SendOperation& op, afl::sys::Timeout_t timeout)
        {
            sendAsync(ctl, op);
            if (!ctl.wait(op, timeout)) {
                cancel(ctl, op);
                return false;
            } else {
                return true;
            }
        }

    virtual void sendAsync(Controller& ctl, SendOperation& op)
        { m_fromMe->sendAsync(ctl, op); }

    virtual bool receive(Controller& ctl, ReceiveOperation& op, afl::sys::Timeout_t timeout)
        {
            receiveAsync(ctl, op);
            if (!ctl.wait(op, timeout)) {
                cancel(ctl, op);
                return false;
            } else {
                return true;
            }
        }

    virtual void receiveAsync(Controller& ctl, ReceiveOperation& op)
        { m_fromThem->receiveAsync(ctl, op); }

    virtual void cancel(Controller& ctl, Operation& op)
        {
            m_fromMe->cancel(ctl, op);
            m_fromThem->cancel(ctl, op);
            ctl.revertPost(op);
        }
    virtual String_t getName()
        { return m_fromMe->getName().toString(); }
    virtual void closeSend()
        { m_fromMe->closeSend(); }
    virtual Name getPeerName()
        { return m_fromThem->getName(); }

 private:
    Ref<Stream> m_fromMe;
    Ref<Stream> m_fromThem;
};

/*
 *  InternalListener: implementation of Listener
 *
 *  Each instance represents one name ("port").
 *  We collect accept() and connect(), and attempt to pair these.
 *  Once we have a pair, we set up the InternalSocket pair.
 *
 *  As long as the InternalListener exists, connect() will be accepted.
 *  We therefore need to know our originating InternalNetworkStack,
 *  to be able to deregister ("close port").
 *  This is the reason why InternalNetworkStack must be heap-allocated.
 */

class afl::net::InternalNetworkStack::InternalListener : public afl::net::Listener {
 public:
    InternalListener(InternalNetworkStack& parent, const Name& name);
    ~InternalListener();

    virtual afl::base::Ptr<Socket> accept(afl::sys::Timeout_t timeout);
    virtual void acceptAsync(Controller& ctl, AcceptOperation& op);
    virtual void cancel(Controller& ctl, Operation& op);
    void connectAsync(Controller& ctl, AcceptOperation& op);

 private:
    const Ref<InternalNetworkStack> m_parent;
    const Name m_name;

    std::vector<AcceptOperation*> m_accepts;
    std::vector<AcceptOperation*> m_connects;

    void tryConnect();
    void cancelAll(std::vector<AcceptOperation*>& vec);
};

inline
afl::net::InternalNetworkStack::InternalListener::InternalListener(InternalNetworkStack& parent, const Name& name)
    : m_parent(parent),
      m_name(name)
{
    m_parent->addListener(m_name, *this);
}

afl::net::InternalNetworkStack::InternalListener::~InternalListener()
{
    MutexGuard g(m_parent->m_mutex);

    m_parent->removeListener(m_name);
    cancelAll(m_accepts);
    cancelAll(m_connects);
}

inline afl::base::Ptr<afl::net::Socket>
afl::net::InternalNetworkStack::InternalListener::accept(afl::sys::Timeout_t timeout)
{
    Controller ctl;
    AcceptOperation op;
    acceptAsync(ctl, op);
    if (ctl.wait(op, timeout)) {
        return op.getResult();
    } else {
        cancel(ctl, op);
        return 0;
    }
}

void
afl::net::InternalNetworkStack::InternalListener::acceptAsync(Controller& ctl, AcceptOperation& op)
{
    MutexGuard g(m_parent->m_mutex);
    op.setController(&ctl);
    m_accepts.push_back(&op);
    tryConnect();
}

inline void
afl::net::InternalNetworkStack::InternalListener::cancel(Controller& ctl, Operation& op)
{
    MutexGuard g(m_parent->m_mutex);
    removeElement(m_accepts, op);
    removeElement(m_connects, op);
    ctl.revertPost(op);
}

inline void
afl::net::InternalNetworkStack::InternalListener::connectAsync(Controller& ctl, AcceptOperation& op)
{
    MutexGuard g(m_parent->m_mutex);
    op.setController(&ctl);
    m_connects.push_back(&op);
    tryConnect();
}

void
afl::net::InternalNetworkStack::InternalListener::tryConnect()
{
    if (!m_accepts.empty() && !m_connects.empty()) {
        // Pair an accept() and a connect()
        AcceptOperation* a = m_accepts.front();
        AcceptOperation* c = m_connects.front();
        m_accepts.erase(m_accepts.begin());
        m_connects.erase(m_connects.begin());

        // Make a socketpair
        SocketPair_t p = InternalNetworkStack::createSocketPair(m_name);
        a->setResult(p.first.asPtr());
        c->setResult(p.second.asPtr());

        // Produce results
        a->getController()->post(*a);
        c->getController()->post(*c);
    }
}

void
afl::net::InternalNetworkStack::InternalListener::cancelAll(std::vector<AcceptOperation*>& vec)
{
    for (std::vector<AcceptOperation*>::iterator it = vec.begin(); it != vec.end(); ++it) {
        AcceptOperation* op = *it;
        op->setResult(0);
        op->getController()->post(*op);
    }
}


/*
 *  InternalNetworkStack
 */

afl::base::Ref<afl::net::InternalNetworkStack>
afl::net::InternalNetworkStack::create()
{
    return *new InternalNetworkStack();
}

afl::net::InternalNetworkStack::SocketPair_t
afl::net::InternalNetworkStack::createSocketPair(const Name& name)
{
    // Two streams
    Ref<Stream> a(*new Stream(name));
    Ref<Stream> b(*new Stream(name));

    // Two cross-connected sockets
    return SocketPair_t(*new InternalSocket(a, b), *new InternalSocket(b, a));
}

inline
afl::net::InternalNetworkStack::InternalNetworkStack()
    : m_mutex(),
      m_listeners()
{ }

afl::base::Ref<afl::net::Listener>
afl::net::InternalNetworkStack::listen(const Name& name, int /*backlogSize*/)
{
    // Ignore the backlogSize for now
    return *new InternalListener(*this, name);
}

afl::base::Ref<afl::net::Socket>
afl::net::InternalNetworkStack::connect(const Name& name, afl::sys::Timeout_t timeout)
{
    AcceptOperation op;
    Controller ctl;

    // Queue connect operation under mutex protection
    // to ensure correct synchronisation in case InternalListener is destructing in parallel.
    {
        MutexGuard g(m_mutex);
        const std::map<String_t, InternalListener*>::iterator it = m_listeners.find(name.toString());
        if (it == m_listeners.end()) {
            throw FileProblemException(name.toString(), "InternalNetworkStack: Connection refused");
        }
        it->second->connectAsync(ctl, op);
    }

    // Wait. If InternalListener died in parallel, it posts a null result.
    if (ctl.wait(op, timeout)) {
        if (op.getResult().get() != 0) {
            return *op.getResult();
        }
    } else {
        // If we're here, the listener is (probably) still alive; look it up again.
        // Failure mode: listener dies right here. Then it will cancel our operation in parallel
        //   and we don't find it, which does not change the overall result.
        //   We just need to get rid of the post.
        // Failure mode: listener dies and is re-built. Then we will find one, but its cancel
        //   operation will be a no-op.
        MutexGuard g(m_mutex);
        const std::map<String_t, InternalListener*>::iterator it = m_listeners.find(name.toString());
        if (it != m_listeners.end()) {
            it->second->cancel(ctl, op);
        } else {
            ctl.revertPost(op);
        }
    }

    throw FileProblemException(name.toString(), "InternalNetworkStack: Connection timed out");
}

void
afl::net::InternalNetworkStack::addListener(const Name& name, InternalListener& lis)
{
    MutexGuard g(m_mutex);
    if (!m_listeners.insert(std::make_pair(name.toString(), &lis)).second) {
        throw FileProblemException(name.toString(), "InternalNetworkStack: Network address already in use");
    }
}

void
afl::net::InternalNetworkStack::removeListener(const Name& name)
{
    MutexGuard g(m_mutex);
    const std::map<String_t, InternalListener*>::iterator it = m_listeners.find(name.toString());
    if (it != m_listeners.end()) {
        m_listeners.erase(it);
    }
}
