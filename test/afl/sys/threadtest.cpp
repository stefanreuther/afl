/**
  *  \file test/afl/sys/threadtest.cpp
  *  \brief Test for afl::sys::Thread
  */

#include "afl/sys/thread.hpp"

#include "afl/base/stoppable.hpp"
#include "afl/sys/time.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.sys.Thread:start", a)
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
        MyRunnable r;
        a.check("01. get", !r.get());

        afl::sys::Thread t("TestSysThread", r);
        a.check("02. get", !r.get());

        t.start();
        t.join();
        a.check("03. get", r.get());
    }

    // Test "String_t" signature
    {
        MyRunnable r;
        a.check("11. get", !r.get());

        afl::sys::Thread t(String_t("TestSysThread"), r);
        a.check("12. get", !r.get());

        t.start();
        t.join();
        a.check("13. get", r.get());
    }
}

/** Test sleep function. */
AFL_TEST("afl.sys.Thread:sleep", a)
{
    uint32_t start = afl::sys::Time::getTickCounter();
    afl::sys::Thread::sleep(300);
    uint32_t elapsed = afl::sys::Time::getTickCounter() - start;

    a.check("min wait", elapsed > 100);
    a.check("max wait", elapsed < 500);
}
