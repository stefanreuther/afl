/**
  *  \file u/t_async_semaphore.cpp
  *  \brief Test for afl::async::Semaphore
  */

#include "afl/async/semaphore.hpp"

#include "u/t_async.hpp"
#include "afl/base/stoppable.hpp"
#include "afl/sys/thread.hpp"
#include "afl/async/operation.hpp"
#include "afl/async/controller.hpp"

namespace {
    // Three semaphores
    struct Three {
        afl::async::Semaphore m_ping;
        afl::async::Semaphore m_pong;
        afl::async::Semaphore m_exit;
    };

    // A semaphore relay thread
    class Relay : public afl::base::Stoppable {
     public:
        Relay(Three& t)
            : m_three(t)
            { }
        void run()
            {
                afl::async::Controller ctl;
                afl::async::Operation pingOp, exitOp;
                m_three.m_ping.waitAsync(ctl, pingOp);
                m_three.m_exit.waitAsync(ctl, exitOp);
                while (1) {
                    afl::async::Operation* op = ctl.wait(1000);
                    if (op == &pingOp) {
                        m_three.m_pong.post();
                        m_three.m_ping.waitAsync(ctl, pingOp);
                    } else if (op == &exitOp) {
                        m_three.m_ping.cancel(ctl, pingOp);
                        break;
                    } else {
                        TS_ASSERT(0);
                    }
                }
            }
        void stop()
            { }
     private:
        Three& m_three;
        };
}

void
TestAsyncSemaphore::testPingPong()
{
    // Make three semaphores and a thread operating on them
    Three three;
    Relay relay(three);
    afl::sys::Thread t("testPingPong", relay);
    t.start();

    // Feed it
    afl::async::Controller ctl;
    for (uint32_t i = 0; i < 10000; ++i) {
        three.m_ping.post();
        three.m_pong.wait(ctl);
    }
    three.m_exit.post();

    // Finish
    t.join();
}

void
TestAsyncSemaphore::testPingPongMulti()
{
    // Make three semaphores and some threads operating on them
    Three three;
    Relay relay(three);

    const size_t N = 10;
    afl::sys::Thread* pt[N];
    for (size_t i = 0; i < N; ++i) {
        pt[i] = new afl::sys::Thread("testPingPong", relay);
        pt[i]->start();
    }

    // Feed it
    afl::async::Controller ctl;
    for (uint32_t i = 0; i < 20000; ++i) {
        three.m_ping.post();
        three.m_pong.wait(ctl);
    }

    // Tell threads to stop
    for (size_t i = 0; i < N; ++i) {
        three.m_exit.post();
    }

    // Finish
    for (size_t i = 0; i < N; ++i) {
        pt[i]->join();
        delete pt[i];
    }
}
