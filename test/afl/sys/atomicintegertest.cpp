/**
  *  \file test/afl/sys/atomicintegertest.cpp
  *  \brief Test for afl::sys::AtomicInteger
  */

#include "afl/sys/atomicinteger.hpp"

#include "afl/base/stoppable.hpp"
#include "afl/sys/thread.hpp"
#include "afl/test/testrunner.hpp"

/** Test basic operations. */
AFL_TEST("afl.sys.AtomicInteger:basics", a)
{
    afl::sys::AtomicInteger ai(0);
    a.checkEqual("01", uint32_t(ai), 0U);

    uint32_t i = ++ai;
    a.checkEqual("02", i, 1U);
    a.checkEqual("03", uint32_t(ai), 1U);

    i = (ai += 41);
    a.checkEqual("04", i, 42U);
    a.checkEqual("05", uint32_t(ai), 42U);

    i = --ai;
    a.checkEqual("06", i, 41U);
    a.checkEqual("07", uint32_t(ai), 41U);

    i = (ai -= 18);
    a.checkEqual("08", i, 23U);
    a.checkEqual("09", uint32_t(ai), 23U);

    i = (ai = 3);
    a.checkEqual("10", i, 3U);
    a.checkEqual("11", uint32_t(ai), 3U);

    a.check("12", !ai.replaceIfEqual(5, 7));
    a.checkEqual("13", uint32_t(ai), 3U);

    a.check("14", ai.replaceIfEqual(3, 7));
    a.checkEqual("15", uint32_t(ai), 7U);

    i = (ai -= 8);
    a.checkEqual("16", uint32_t(ai), 0xFFFFFFFFU);
    a.checkEqual("17", i, 0xFFFFFFFFU);
}

/** Test parallel AtomicInteger::operator++ */
AFL_TEST("afl.sys.AtomicInteger:parallel-increment", a)
{
    static const uint32_t LIMIT = 1000000;

    // Content for a thread
    class MyRunner : public afl::base::Stoppable {
     public:
        MyRunner(afl::sys::AtomicInteger& ai)
            : m_ai(ai)
            { }
        void run()
            {
                for (uint32_t i = 0; i < LIMIT; ++i) {
                    ++m_ai;
                }
            }
        void stop()
            { }
     private:
        afl::sys::AtomicInteger& m_ai;
    };

    // Four threads. Since a Runnable has no state, we can run the same in each of them.
    afl::sys::AtomicInteger ai(0);
    MyRunner r(ai);
    afl::sys::Thread t1("t1", r), t2("t2", r), t3("t3", r), t4("t4", r);
    t1.start();
    t2.start();
    t3.start();
    t4.start();
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    a.checkEqual("", uint32_t(ai), 4 * LIMIT);
}

/** Test parallel AtomicInteger::replaceIfEqual (compare-and-set).
    For simplicity, we use replaceIfEqual to implement just the increment. */
AFL_TEST("afl.sys.AtomicInteger:parallel-replace", a)
{
    static const uint32_t LIMIT = 100000;

    // Content for a thread
    class MyRunner : public afl::base::Stoppable {
     public:
        MyRunner(afl::sys::AtomicInteger& ai)
            : m_ai(ai)
            { }
        void run()
            {
                for (uint32_t i = 0; i < LIMIT; ++i) {
                    uint32_t n;
                    do {
                        n = m_ai;
                    } while (!m_ai.replaceIfEqual(n, n+1));
                }
            }
        void stop()
            { }
     private:
        afl::sys::AtomicInteger& m_ai;
    };

    // Four threads. Since a Runnable has no state, we can run the same in each of them.
    afl::sys::AtomicInteger ai(0);
    MyRunner r(ai);
    afl::sys::Thread t1("t1", r), t2("t2", r), t3("t3", r), t4("t4", r);
    t1.start();
    t2.start();
    t3.start();
    t4.start();
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    a.checkEqual("", uint32_t(ai), 4 * LIMIT);
}
