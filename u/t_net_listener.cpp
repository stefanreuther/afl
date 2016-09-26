/**
  *  \file u/t_net_listener.cpp
  *  \brief Test for afl::net::Listener
  */

#include "afl/net/listener.hpp"

#include <cstdio>
#include <cstdlib>
#include "u/t_net.hpp"
#include "afl/async/controller.hpp"
#include "afl/async/messageexchange.hpp"
#include "afl/async/sendoperation.hpp"
#include "afl/base/runnable.hpp"
#include "afl/net/acceptoperation.hpp"
#include "afl/net/name.hpp"
#include "afl/net/networkstack.hpp"
#include "afl/sys/thread.hpp"

/** Test listening with timeout. */
void
TestNetListener::testListen()
{
    afl::net::NetworkStack& ns = afl::net::NetworkStack::getInstance();

    // Pick a random port number
    afl::net::Name name("localhost", uint16_t(std::rand() % 10000 + 20000));

    // Create a listener
    afl::base::Ptr<afl::net::Listener> listener = ns.listen(name, 10);
    TS_ASSERT(listener.get() != 0);

    // Accept a connection, with timeout
    afl::base::Ptr<afl::net::Socket> sock = listener->accept(1000);
    TS_ASSERT(sock.get() == 0);
}

/** Test asynchronous listening.
    This simple test just verifies that acceptAsync() works somehow. */
void
TestNetListener::testAsync()
{
    class Helper : public afl::base::Runnable {
     public:
        Helper()
            : m_out()
            {
                // Pick a port number
                std::sprintf(m_portNr, "%d", std::rand() % 10000 + 20000);
            }
        void run()
            {
                // Prepare network
                afl::base::Ptr<afl::net::Listener> listener
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
                        TS_ASSERT(0);
                    }
                }
            }

        char m_portNr[10];
        afl::async::MessageExchange m_out;
    };

    // Start helper
    Helper helper;
    afl::sys::Thread helperThread("helperThread", helper);
    helperThread.start();

    // Wait for synchronisation
    afl::async::ReceiveOperation recvOp;
    afl::async::Controller ctl;
    TS_ASSERT(helper.m_out.receive(ctl, recvOp, 1000));

    // Connect to network
    afl::base::Ptr<afl::net::Socket> sock(afl::net::NetworkStack::getInstance().connect(afl::net::Name("localhost", helper.m_portNr), 1000));
    TS_ASSERT(sock.get() != 0);

    // Finish
    helperThread.join();
}
