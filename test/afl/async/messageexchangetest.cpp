/**
  *  \file test/afl/async/messageexchangetest.cpp
  *  \brief Test for afl::async::MessageExchange
  */

#include "afl/async/messageexchange.hpp"

#include <stdexcept>
#include "afl/async/controller.hpp"
#include "afl/async/semaphore.hpp"
#include "afl/base/stoppable.hpp"
#include "afl/sys/thread.hpp"
#include "afl/test/testrunner.hpp"

using afl::base::ConstBytes_t;

namespace {
    const size_t LIMIT = 10;

    /** Helper class for testing. */
    class RelayTester : public afl::base::Stoppable {
     public:
        RelayTester(afl::async::MessageExchange& mx)
            : m_sem(),
              m_mx(mx)
            { }

        void run()
            {
                afl::async::Controller ctl;

                // Start
                uint8_t data[LIMIT];
                afl::async::ReceiveOperation rx(data);
                afl::async::Operation semop;
                m_mx.receiveAsync(ctl, rx);
                m_sem.waitAsync(ctl, semop);

                while (1) {
                    afl::async::Operation* p = ctl.wait();
                    if (p == &rx) {
                        afl::async::SendOperation tx(rx.getReceivedBytes());
                        m_mx.send(ctl, tx);

                        rx.setData(data);
                        m_mx.receiveAsync(ctl, rx);
                    } else if (p == &semop) {
                        break;
                    } else {
                        throw std::runtime_error("unexpected");
                    }
                }

                m_mx.cancel(ctl, rx);
            }

        void stop()
            {
                m_sem.post();
            }
     private:
        afl::async::Semaphore m_sem;
        afl::async::MessageExchange& m_mx;
    };
}

/** Test some ping-pong. */
AFL_TEST_NOARG("afl.async.MessageExchange:ping-pong")
{
    // Set up
    afl::async::MessageExchange mx;
    RelayTester t(mx);
    afl::sys::Thread tt("testPingPong", t);
    tt.start();

    // Run
    afl::async::Controller ctl;
    for (uint32_t i = 0; i < 1000; ++i) {
        afl::async::SendOperation tx;
        mx.send(ctl, tx);

        afl::async::ReceiveOperation rx;
        mx.receive(ctl, rx);
    }
    t.stop();
    tt.join();
}

/** Test data transmission behaviour. */
AFL_TEST("afl.async.MessageExchange:data", a)
{
    // Set up
    afl::async::MessageExchange mx;
    RelayTester t(mx);
    afl::sys::Thread tt("testData", t);
    tt.start();

    afl::async::Controller ctl;

    // Send some text. Must get it back.
    {
        static const uint8_t data[] = { 'h', 'e', 'l', 'l', 'o' };
        afl::async::SendOperation tx(data);
        mx.send(ctl, tx);
        a.checkEqual("01. getNumSentBytes", tx.getNumSentBytes(), sizeof(data));

        uint8_t dataBack[10];
        afl::async::ReceiveOperation rx(dataBack);
        mx.receive(ctl, rx);
        a.checkEqual("02. getNumReceivedBytes", rx.getNumReceivedBytes(), sizeof(data));
        a.checkEqualContent("03. data",
                            ConstBytes_t(data),
                            ConstBytes_t(dataBack).trim(sizeof(data)));
    }

    // Send a longer text. It will be truncated by the background thread.
    {
        static const uint8_t data[] = { 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd' };
        a.check("11. limit", sizeof(data) > LIMIT);

        afl::async::SendOperation tx(data);
        mx.send(ctl, tx);
        a.checkEqual("12. getNumSentBytes", tx.getNumSentBytes(), LIMIT);

        uint8_t dataBack[sizeof(data)];
        a.check("13. limit", sizeof(dataBack) > LIMIT);
        afl::async::ReceiveOperation rx(dataBack);
        mx.receive(ctl, rx);
        a.checkEqual("14. getNumReceivedBytes", rx.getNumReceivedBytes(), LIMIT);
        a.checkEqualContent("15. data",
                            ConstBytes_t(data).trim(LIMIT),
                            ConstBytes_t(dataBack).trim(LIMIT));
    }

    // Send some text. Truncates at receiver end.
    {
        static const uint8_t data[] = { 'h', 'e', 'l', 'l', 'o' };
        afl::async::SendOperation tx(data);
        mx.send(ctl, tx);
        a.checkEqual("21. getNumSentBytes", tx.getNumSentBytes(), sizeof(data));

        uint8_t dataBack[3];
        afl::async::ReceiveOperation rx(dataBack);
        mx.receive(ctl, rx);
        a.checkEqual("22. getNumReceivedBytes", rx.getNumReceivedBytes(), sizeof(dataBack));
        a.checkEqualContent("23. data",
                            ConstBytes_t(data).trim(sizeof(dataBack)),
                            ConstBytes_t(dataBack));
    }

    // Stop
    t.stop();
    tt.join();
}
