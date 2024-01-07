/**
  *  \file test/afl/net/listenertest.cpp
  *  \brief Test for afl::net::Listener
  */

#include "afl/net/listener.hpp"

#include <cstdio>
#include <cstdlib>
#include "afl/async/controller.hpp"
#include "afl/async/messageexchange.hpp"
#include "afl/async/sendoperation.hpp"
#include "afl/base/stoppable.hpp"
#include "afl/net/acceptoperation.hpp"
#include "afl/net/name.hpp"
#include "afl/net/networkstack.hpp"
#include "afl/net/socket.hpp"
#include "afl/sys/thread.hpp"
#include "afl/test/testrunner.hpp"

/** Test listening with timeout. */
AFL_TEST("afl.net.Listener:listen+accept", a)
{
    afl::net::NetworkStack& ns = afl::net::NetworkStack::getInstance();

    // Pick a random port number
    afl::net::Name name("localhost", uint16_t(std::rand() % 10000 + 20000));

    // Create a listener
    afl::base::Ref<afl::net::Listener> listener = ns.listen(name, 10);
    a.checkNonNull("listener", &listener.get());

    // Accept a connection, with timeout
    afl::base::Ptr<afl::net::Socket> sock = listener->accept(1000);
    a.checkNull("socket", sock.get());
}

/** Test asynchronous listening.
    This simple test just verifies that acceptAsync() works somehow. */
AFL_TEST("afl.net.Listener:acceptAsync", a)
{
    class Helper : public afl::base::Stoppable {
     public:
        Helper(afl::test::Assert a)
            : m_out(), m_assert(a)
            {
                // Pick a port number
                std::sprintf(m_portNr, "%d", std::rand() % 10000 + 20000);
            }
        void run()
            {
                // Prepare network
                afl::base::Ref<afl::net::Listener> listener
                    (afl::net::NetworkStack::getInstance().listen(afl::net::Name("localhost", m_portNr), 10));

                // Start operations
                afl::async::SendOperation sendOp;
                afl::net::AcceptOperation acceptOp;
                afl::async::Controller ctl;

                listener->acceptAsync(ctl, acceptOp);
                m_out.sendAsync(ctl, sendOp);

                // Wait for completion.
                bool didAccept = false;
                bool didSend = false;
                while (!didAccept || !didSend) {
                    afl::async::Operation* op = ctl.wait(1000);
                    if (op == &sendOp) {
                        didSend = true;
                    } else if (op == &acceptOp) {
                        didAccept = true;
                    } else {
                        m_assert.fail("unexpected");
                    }
                }
            }
        void stop()
            { }

        char m_portNr[10];
        afl::async::MessageExchange m_out;
        afl::test::Assert m_assert;
    };

    // Start helper
    Helper helper(a("background thread"));
    afl::sys::Thread helperThread("helperThread", helper);
    helperThread.start();

    // Wait for synchronisation
    afl::async::ReceiveOperation recvOp;
    afl::async::Controller ctl;
    a.check("thread started", helper.m_out.receive(ctl, recvOp, 1000));

    // Connect to network
    afl::base::Ref<afl::net::Socket> sock(afl::net::NetworkStack::getInstance().connect(afl::net::Name("localhost", helper.m_portNr), 1000));
    a.checkNonNull("connect success", &sock.get());

    // Finish
    helperThread.join();
}
