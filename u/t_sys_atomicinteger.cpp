/**
  *  \file u/t_sys_atomicinteger.cpp
  *  \brief Test for afl::sys::AtomicInteger
  */

#include "afl/sys/atomicinteger.hpp"

#include "u/t_sys.hpp"
#include "afl/base/runnable.hpp"
#include "afl/sys/thread.hpp"

/** Test basic operations. */
void
TestSysAtomicInteger::testBasics()
{
    afl::sys::AtomicInteger ai(0);
    TS_ASSERT_EQUALS(ai, 0U);

    uint32_t i = ++ai;
    TS_ASSERT_EQUALS(i, 1U);
    TS_ASSERT_EQUALS(ai, 1U);

    i = (ai += 41);
    TS_ASSERT_EQUALS(i, 42U);
    TS_ASSERT_EQUALS(ai, 42U);

    i = --ai;
    TS_ASSERT_EQUALS(i, 41U);
    TS_ASSERT_EQUALS(ai, 41U);

    i = (ai -= 18);
    TS_ASSERT_EQUALS(i, 23U);
    TS_ASSERT_EQUALS(ai, 23U);

    i = (ai = 3);
    TS_ASSERT_EQUALS(i, 3U);
    TS_ASSERT_EQUALS(ai, 3U);

    TS_ASSERT(!ai.replaceIfEqual(5, 7));
    TS_ASSERT_EQUALS(ai, 3U);

    TS_ASSERT(ai.replaceIfEqual(3, 7));
    TS_ASSERT_EQUALS(ai, 7U);

    i = (ai -= 8);
    TS_ASSERT_EQUALS(ai, 0xFFFFFFFFU);
    TS_ASSERT_EQUALS(i, 0xFFFFFFFFU);
}

/** Test AtomicInteger::operator++ */
void
TestSysAtomicInteger::testParallelIncrement()
{
    static const uint32_t LIMIT = 1000000;

    // Content for a thread
    class MyRunner : public afl::base::Runnable {
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
    TS_ASSERT_EQUALS(ai, 4 * LIMIT);
}

/** Test AtomicInteger::replaceIfEqual (compare-and-set).
    For simplicity, we use replaceIfEqual to implement just the increment. */
void
TestSysAtomicInteger::testParallelReplace()
{
    static const uint32_t LIMIT = 100000;

    // Content for a thread
    class MyRunner : public afl::base::Runnable {
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
    TS_ASSERT_EQUALS(ai, 4 * LIMIT);
}
