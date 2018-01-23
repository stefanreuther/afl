/**
  *  \file u/t_async_mutex.cpp
  *  \brief Test for afl::async::Mutex
  */

#include "afl/async/mutex.hpp"

#include "u/t_async.hpp"
#include "afl/sys/thread.hpp"
#include "afl/sys/semaphore.hpp"
#include "afl/base/stoppable.hpp"
#include "afl/async/controller.hpp"

void
TestAsyncMutex::testSimple()
{
    static const uint32_t N = 10000;

    // Testee
    struct Testee {
        volatile uint32_t m_value;
        afl::async::Mutex m_mutex;

        Testee()
            : m_value(0),
              m_mutex()
            { }
    };
    Testee testee;

    // Tester threads
    class Tester : public afl::base::Stoppable {
     public:
        Tester(Testee& t)
            : m_testee(t)
            { }

        void run()
            {
                afl::async::Controller ctl;
                for (uint32_t i = 0; i < N; ++i) {
                    m_testee.m_mutex.wait(ctl);
                    m_testee.m_value++;
                    m_testee.m_mutex.post(ctl);
                }
            }
        void stop()
            { }
     private:
        Testee& m_testee;
    };
    Tester tester(testee);

    afl::sys::Thread t1("TestAsyncMutex::testSimple #1", tester);
    afl::sys::Thread t2("TestAsyncMutex::testSimple #2", tester);
    afl::sys::Thread t3("TestAsyncMutex::testSimple #3", tester);
    t1.start();
    t2.start();
    t3.start();
    t1.join();
    t2.join();
    t3.join();

    TS_ASSERT_EQUALS(testee.m_value, 3*N);
}

/** Test acquiring a mutex multiple times. */
void
TestAsyncMutex::testMulti()
{
    // Start helper thread
    class Tester : public afl::base::Stoppable {
     public:
        Tester()
            : m_stepAux(0),
              m_stepMain(0),
              m_testee()
            { }

        void run()
            {
                // Obtain the mutex and tell the caller
                afl::async::Controller ctl;
                m_testee.wait(ctl);
                m_stepMain.post();

                // Release the mutex when caller says so
                m_stepAux.wait();
                m_testee.post(ctl);

                // Terminate when caller says so
                m_stepAux.wait();
            }

        void stop()
            { }

        afl::sys::Semaphore m_stepAux;
        afl::sys::Semaphore m_stepMain;
        afl::async::Mutex m_testee;
    };
    Tester t;
    afl::sys::Thread tt("TestAsyncMutex::testMulti", t);
    tt.start();

    // Wait until thread started up
    t.m_stepMain.wait();

    // Asynchronously attempt to obtain the mutex
    afl::async::Controller ctl;
    afl::async::Operation op1, op2;
    t.m_testee.waitAsync(ctl, op1);
    t.m_testee.waitAsync(ctl, op2);
    TS_ASSERT(ctl.wait(100) == 0);

    // Tell thread to release the mutex
    t.m_stepAux.post();

    // Now we must be able to obtain it
    bool did1 = false, did2 = false;
    while (afl::async::Operation* p = ctl.wait(100)) {
        if (p == &op1) {
            TS_ASSERT(!did1);
            did1 = true;
        } else if (p == &op2) {
            TS_ASSERT(!did2);
            did2 = true;
        } else {
            TS_ASSERT(0);
        }
    }
    TS_ASSERT(did1);
    TS_ASSERT(did2);

    // Terminate the thread
    t.m_stepAux.post();
    tt.join();
}
