/**
  *  \file test/afl/net/internalnetworkstacktest.cpp
  */

#include "afl/net/internalnetworkstack.hpp"

#include <stdexcept>
#include "afl/async/controller.hpp"
#include "afl/async/receiveoperation.hpp"
#include "afl/async/sendoperation.hpp"
#include "afl/base/stoppable.hpp"
#include "afl/net/acceptoperation.hpp"
#include "afl/sys/semaphore.hpp"
#include "afl/sys/thread.hpp"
#include "afl/sys/time.hpp"
#include "afl/test/testrunner.hpp"

using afl::async::Controller;
using afl::async::Operation;
using afl::async::ReceiveOperation;
using afl::async::SendOperation;
using afl::base::Ptr;
using afl::base::Ref;
using afl::net::AcceptOperation;
using afl::net::InternalNetworkStack;
using afl::net::Listener;
using afl::net::Name;
using afl::net::Socket;
using afl::sys::Semaphore;
using afl::sys::Thread;
using afl::sys::Time;
using afl::test::Assert;

/* Test a simple data transfer using async functions */
AFL_TEST("afl.net.InternalNetworkStack:simple-transfer:async", a)
{
    Ref<InternalNetworkStack> stack = InternalNetworkStack::create();
    Name name("host", "service");

    // Asynchronous listen
    Controller ctl;
    Ref<Listener> listener = stack->listen(name, 10);
    AcceptOperation aop;
    listener->acceptAsync(ctl, aop);

    // Connect
    Ref<Socket> csock = stack->connect(name, 1000);

    // Connect causes accept to succeed
    Operation* op = ctl.wait(1000);
    a.checkEqual("01. accept wait", op, &aop);
    a.checkNonNull("02. accept sock", aop.getResult().get());

    // We now have two sockets, csock and aop.getResult().
    // For now, we guarantee the name of the accept-side endpoint (which is connect's peer).
    a.checkEqual("11. name", csock->getPeerName().toString(), name.toString());
    a.checkEqual("12. name", aop.getResult()->getName(), name.toString());

    // Send some data.
    static const uint8_t dataToSend[] = {5,6,7,8};
    uint8_t dataToReceive[4] = {0,0,0,0};

    SendOperation sendOp(dataToSend);
    ReceiveOperation recvOp(dataToReceive);
    csock->sendAsync(ctl, sendOp);
    aop.getResult()->receiveAsync(ctl, recvOp);

    Operation* op1 = ctl.wait(1000);
    Operation* op2 = ctl.wait(1000);
    a.checkNonNull("21. wait 1", op1);
    a.checkNonNull("22. wait 2", op2);
    if (op1 == &sendOp) {
        a.checkEqual("23. wait result", op2, &recvOp);
    } else {
        a.checkEqual("24. wait result", op1, &recvOp);
        a.checkEqual("25. wait result", op2, &sendOp);
    }

    // Verify transfer result
    a.checkEqual("31. send count", sendOp.getNumSentBytes(), 4U);
    a.checkEqual("32. recv count", recvOp.getNumReceivedBytes(), 4U);
    a.checkEqualContent("33. data", afl::base::ConstBytes_t(dataToSend), afl::base::ConstBytes_t(dataToReceive));
}

/* Test a simple data transfer using synchronous functions */
AFL_TEST("afl.net.InternalNetworkStack:simple-transfer:sync", a)
{
    static const uint8_t dataToSend[] = {77,88,99};

    class ListenTask : public afl::base::Stoppable {
     public:
        ListenTask(Ref<InternalNetworkStack> stack, Name name, Assert a)
            : m_started(0),
              m_stack(stack),
              m_name(name),
              m_assert(a)
            { }

        void run()
            {
                // Accept
                Ref<Listener> listener = m_stack->listen(m_name, 10);
                m_started.post();
                Ptr<Socket> asock = listener->accept(1000);
                m_assert.checkNonNull("01. accept", asock.get());

                // Send
                Controller ctl;
                SendOperation sendOp(dataToSend);
                bool sendOK = asock->send(ctl, sendOp, 1000);
                m_assert.check("11. send ok", sendOK);
                m_assert.checkEqual("12. sent", sendOp.getNumSentBytes(), sizeof(dataToSend));
                m_assert.checkEqual("13. sent", sendOp.getUnsentBytes().size(), 0U);
            }

        void stop()
            { }

        void waitForStartup()
            { m_started.wait(); }

     private:
        Semaphore m_started;
        Ref<InternalNetworkStack> m_stack;
        Name m_name;
        Assert m_assert;
    };

    // Objects
    Ref<InternalNetworkStack> stack = InternalNetworkStack::create();
    Name name("host", "service");

    // Listen in background thread
    ListenTask task(stack, name, a("background"));
    Thread thread("thread", task);
    thread.start();
    task.waitForStartup();

    // Connect
    Ref<Socket> csock = stack->connect(name, 1000);

    // Receive
    uint8_t dataToReceive[100] = {0};
    Controller ctl;
    ReceiveOperation recvOp(dataToReceive);
    bool recvOK = csock->receive(ctl, recvOp, 1000);
    a.check("21. recv ok", recvOK);
    a.checkEqual("22. bytes", recvOp.getNumReceivedBytes(), sizeof(dataToSend));

    a.checkEqual("23. data", dataToSend[0], dataToReceive[0]);
}

/* Test closeSend */
AFL_TEST("afl.net.InternalNetworkStack:closeSend", a)
{
    Ref<InternalNetworkStack> stack = InternalNetworkStack::create();
    Name name("host", "service");

    // Asynchronous listen
    Controller ctl;
    Ref<Listener> listener = stack->listen(name, 10);
    AcceptOperation aop;
    listener->acceptAsync(ctl, aop);

    // Connect
    Ref<Socket> csock = stack->connect(name, 1000);

    // Connect causes accept to succeed
    Operation* op = ctl.wait(1000);
    a.checkEqual("01. accept wait", op, &aop);
    a.checkNonNull("02. accept sock", aop.getResult().get());

    // Try to receive; hangs for now
    uint8_t dataToReceive[4] = {0,0,0,0};
    ReceiveOperation recvOp(dataToReceive);
    aop.getResult()->receiveAsync(ctl, recvOp);
    a.checkNull("11. receiveAsync", ctl.wait(100));

    // Close sender end; receive completes now
    csock->closeSend();
    Operation* op1 = ctl.wait(1000);
    a.checkEqual("21. wait result", op1, &recvOp);
    a.checkEqual("22. recv count", recvOp.getNumReceivedBytes(), 0U);
}

/* Test reception timeout */
AFL_TEST("afl.net.InternalNetworkStack:receive-timeout", a)
{
    Ref<InternalNetworkStack> stack = InternalNetworkStack::create();
    Name name("host", "service");

    // Asynchronous listen
    Controller ctl;
    Ref<Listener> listener = stack->listen(name, 10);
    AcceptOperation aop;
    listener->acceptAsync(ctl, aop);

    // Connect
    Ref<Socket> csock = stack->connect(name, 1000);
    uint8_t dataToReceive[] = {0,0,0};
    ReceiveOperation recvOp(dataToReceive);
    bool recvOK = csock->receive(ctl, recvOp, 100);
    a.check("01. recv", !recvOK);
}

/* Test send timeout */
AFL_TEST("afl.net.InternalNetworkStack:send-timeout", a)
{
    Ref<InternalNetworkStack> stack = InternalNetworkStack::create();
    Name name("host", "service");

    // Asynchronous listen
    Controller ctl;
    Ref<Listener> listener = stack->listen(name, 10);
    AcceptOperation aop;
    listener->acceptAsync(ctl, aop);

    // Connect
    Ref<Socket> csock = stack->connect(name, 1000);
    uint8_t dataToSend[] = {8,9,10};
    SendOperation sendOp(dataToSend);
    bool sendOK = csock->send(ctl, sendOp, 100);
    a.check("01. send", !sendOK);
}

/* Error case: accept times out, no connect */
AFL_TEST("afl.net.InternalNetworkStack:error:accept", a)
{
    Ref<InternalNetworkStack> stack = InternalNetworkStack::create();
    Name name("host", "service");

    Controller ctl;
    Ref<Listener> listener = stack->listen(name, 10);

    Ptr<Socket> sock = listener->accept(100);

    a.checkNull("01. accept", sock.get());
}

/* Error case: connect fails, no listener */
AFL_TEST("afl.net.InternalNetworkStack:error:connect:no-listener", a)
{
    Ref<InternalNetworkStack> stack = InternalNetworkStack::create();
    Name name("host", "service");

    uint32_t t0 = Time::getTickCounter();
    AFL_CHECK_THROWS(a, stack->connect(name, 1000), std::runtime_error);
    uint32_t t1 = Time::getTickCounter();

    // Verify that this is NOT a timeout
    a.checkLessThan("elapsed time", t1 - t0, 50U);
}

/* Error case: connect times out, no accept */
AFL_TEST("afl.net.InternalNetworkStack:error:connect:no-accept", a)
{
    Ref<InternalNetworkStack> stack = InternalNetworkStack::create();
    Name name("host", "service");

    Ref<Listener> listener = stack->listen(name, 10);

    uint32_t t0 = Time::getTickCounter();
    AFL_CHECK_THROWS(a, stack->connect(name, 100), std::runtime_error);
    uint32_t t1 = Time::getTickCounter();

    // Verify that this actually is a timeout
    a.checkGreaterThan("elapsed time", t1 - t0, 50U);
}

/* Error case: connect canceled by deleted listener */
AFL_TEST("afl.net.InternalNetworkStack:error:connect:delete-listener", a)
{
    class ListenTask : public afl::base::Stoppable {
     public:
        ListenTask(Ref<InternalNetworkStack> stack, Name name, Assert a)
            : m_started(0),
              m_stack(stack),
              m_name(name),
              m_assert(a)
            { }

        void run()
            {
                // Listen
                Ref<Listener> listener = m_stack->listen(m_name, 10);
                m_started.post();

                // Sleep; then discard the listener. This will release the connect()
                Thread::sleep(100);
            }

        void stop()
            { }

        void waitForStartup()
            { m_started.wait(); }

     private:
        Semaphore m_started;
        Ref<InternalNetworkStack> m_stack;
        Name m_name;
        Assert m_assert;
    };


    Ref<InternalNetworkStack> stack = InternalNetworkStack::create();
    Name name("host", "service");

    ListenTask task(stack, name, a("background"));
    Thread thread("thread", task);
    thread.start();
    task.waitForStartup();

    uint32_t t0 = Time::getTickCounter();
    AFL_CHECK_THROWS(a, stack->connect(name, 1000), std::runtime_error);
    uint32_t t1 = Time::getTickCounter();

    // Verify that this actually is a timeout
    a.checkLessThan("elapsed time", t1 - t0, 500U);
}

/* Error case: multiple listeners */
AFL_TEST("afl.net.InternalNetworkStack:error:address-in-use", a)
{
    Ref<InternalNetworkStack> stack = InternalNetworkStack::create();
    Ref<Listener> listener = stack->listen(Name("host", "service"), 10);
    AFL_CHECK_THROWS(a, stack->listen(Name("host", "service"), 10), std::runtime_error);
}

/* Multiple listeners but no conflict */
AFL_TEST("afl.net.InternalNetworkStack:reuse-address", a)
{
    Ref<InternalNetworkStack> stack = InternalNetworkStack::create();

    // This line creates and discards the listener.
    // Thus, the same line again can create another one.
    AFL_CHECK_SUCCEEDS(a("first"),  stack->listen(Name("host", "service"), 10));
    AFL_CHECK_SUCCEEDS(a("second"), stack->listen(Name("host", "service"), 10));
}

/* Standalone socket pair */
AFL_TEST("afl.net.InternalNetworkStack:createSocketPair", a)
{
    InternalNetworkStack::SocketPair_t p = InternalNetworkStack::createSocketPair(Name("a", "b"));
    Controller ctl;

    static const uint8_t dataToSend[] = {32,16,8};
    SendOperation sendOp(dataToSend);
    p.first->sendAsync(ctl, sendOp);

    uint8_t dataToReceive[10];
    ReceiveOperation recvOp(dataToReceive);
    bool ok = p.second->receive(ctl, recvOp);
    a.checkEqual("01. receive ok", ok, true);
    a.checkEqual("02. receive bytes", recvOp.getNumReceivedBytes(), sizeof(dataToSend));
    a.checkEqualContent("03. receive data", recvOp.getReceivedBytes(), afl::base::ConstBytes_t(dataToSend));
}
