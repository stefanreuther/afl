/**
  *  \file u/t_sys_thread.cpp
  *  \brief Test for afl::sys::Thread
  */

#include "afl/sys/thread.hpp"

#include "u/t_sys.hpp"
#include "afl/base/stoppable.hpp"
#include "afl/sys/time.hpp"

/** Simple test. */
void
TestSysThread::testIt()
{
    class MyRunnable : public afl::base::Stoppable {
     public:
        MyRunnable()
            : m_value(false)
            { }
        void run()
            { m_value = true; }
        bool get()
            { return m_value; }
        void stop()
            { }
     private:
        bool m_value;
    };

    // Test "const char*" signature
    {
        MyRunnable a;
        TS_ASSERT(!a.get());

        afl::sys::Thread t("TestSysThread", a);
        TS_ASSERT(!a.get());

        t.start();
        t.join();
        TS_ASSERT(a.get());
    }

    // Test "String_t" signature
    {
        MyRunnable a;
        TS_ASSERT(!a.get());

        afl::sys::Thread t(String_t("TestSysThread"), a);
        TS_ASSERT(!a.get());

        t.start();
        t.join();
        TS_ASSERT(a.get());
    }
}

/** Test sleep function. */
void
TestSysThread::testSleep()
{
    uint32_t a = afl::sys::Time::getTickCounter();
    afl::sys::Thread::sleep(300);
    uint32_t elapsed = afl::sys::Time::getTickCounter() - a;

    TS_ASSERT(elapsed > 100);
    TS_ASSERT(elapsed < 500);
}
