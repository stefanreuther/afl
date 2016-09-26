/**
  *  \file u/t_async_messageexchange.cpp
  *  \brief Test for afl::async::MessageExchange
  */

#include "afl/async/messageexchange.hpp"

#include "u/t_async.hpp"
#include "afl/base/runnable.hpp"
#include "afl/async/controller.hpp"
#include "afl/sys/thread.hpp"
#include "afl/async/semaphore.hpp"

namespace {
    const size_t LIMIT = 10;

    /** Helper class for testing. */
    class RelayTester : public afl::base::Runnable {
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
                        TS_ASSERT(0);
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
void
TestAsyncMessageExchange::testPingPong()
{
    // Set up
    afl::async::MessageExchange mx;
    RelayTester t(mx);
    afl::sys::Thread tt("TestAsyncMessageExchange::testPingPong", t);
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
void
TestAsyncMessageExchange::testData()
{
    // Set up
    afl::async::MessageExchange mx;
    RelayTester t(mx);
    afl::sys::Thread tt("TestAsyncMessageExchange::testPingPong", t);
    tt.start();

    afl::async::Controller ctl;

    // Send some text. Must get it back.
    {
        static const uint8_t data[] = { 'h', 'e', 'l', 'l', 'o' };
        afl::async::SendOperation tx(data);
        mx.send(ctl, tx);
        TS_ASSERT_EQUALS(tx.getNumSentBytes(), sizeof(data));

        uint8_t dataBack[10];
        afl::async::ReceiveOperation rx(dataBack);
        mx.receive(ctl, rx);
        TS_ASSERT_EQUALS(rx.getNumReceivedBytes(), sizeof(data));
        TS_ASSERT_SAME_DATA(data, dataBack, sizeof(data));
    }

    // Send a longer text. It will be truncated by the background thread.
    {
        static const uint8_t data[] = { 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd' };
        TS_ASSERT(sizeof(data) > LIMIT);

        afl::async::SendOperation tx(data);
        mx.send(ctl, tx);
        TS_ASSERT_EQUALS(tx.getNumSentBytes(), LIMIT);

        uint8_t dataBack[sizeof(data)];
        TS_ASSERT(sizeof(dataBack) > LIMIT);
        afl::async::ReceiveOperation rx(dataBack);
        mx.receive(ctl, rx);
        TS_ASSERT_EQUALS(rx.getNumReceivedBytes(), LIMIT);
        TS_ASSERT_SAME_DATA(data, dataBack, LIMIT);
    }

    // Send some text. Truncates at receiver end.
    {
        static const uint8_t data[] = { 'h', 'e', 'l', 'l', 'o' };
        afl::async::SendOperation tx(data);
        mx.send(ctl, tx);
        TS_ASSERT_EQUALS(tx.getNumSentBytes(), sizeof(data));

        uint8_t dataBack[3];
        afl::async::ReceiveOperation rx(dataBack);
        mx.receive(ctl, rx);
        TS_ASSERT_EQUALS(rx.getNumReceivedBytes(), sizeof(dataBack));
        TS_ASSERT_SAME_DATA(data, dataBack, sizeof(dataBack));
    }

    // Stop
    t.stop();
    tt.join();
}
