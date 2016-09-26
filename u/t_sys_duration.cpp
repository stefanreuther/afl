/**
  *  \file u/t_sys_duration.cpp
  *  \brief Test for afl::sys::Duration
  */

#include "afl/sys/duration.hpp"
#include "u/t_sys.hpp"
#include "afl/sys/parsedtime.hpp"

/** Test construction and deconstruction. */
void
TestSysDuration::testConstruction()
{
    using afl::sys::Duration;

    // 0
    TS_ASSERT_EQUALS(Duration().getMilliseconds(), 0);
    TS_ASSERT(Duration().isValid());

    // 999
    TS_ASSERT_EQUALS(Duration(999).getMilliseconds(), 999);
    TS_ASSERT(Duration(999).isValid());

    // Negative
    TS_ASSERT_EQUALS(Duration(-42).getMilliseconds(), -42);
    TS_ASSERT(!Duration(-42).isValid());

    // Units
    TS_ASSERT_EQUALS(Duration::fromSeconds(3).getMilliseconds(), 3000);
    TS_ASSERT_EQUALS(Duration::fromMinutes(7).getMilliseconds(), 7LL*60*1000);
    TS_ASSERT_EQUALS(Duration::fromHours(2).getMilliseconds(), 2LL*60*60*1000);

    // ParsedTime
    {
        afl::sys::ParsedTime pt = afl::sys::ParsedTime();
        pt.m_minute = 2;
        TS_ASSERT_EQUALS(Duration(pt).getMilliseconds(), 2LL*60*1000);
    }
    {
        afl::sys::ParsedTime pt = afl::sys::ParsedTime();
        pt.m_day = 99;
        pt.m_millisecond = 9;
        TS_ASSERT_EQUALS(Duration(pt).getMilliseconds(), 8553600009LL);
    }
    {
        afl::sys::ParsedTime pt = afl::sys::ParsedTime();
        pt.m_minute = 100;
        TS_ASSERT_EQUALS(Duration(pt).getMilliseconds(), 6000000LL);
    }
    {
        afl::sys::ParsedTime pt;
        Duration(8553600009LL).unpack(pt);
        TS_ASSERT_EQUALS(pt.m_year, 0);
        TS_ASSERT_EQUALS(pt.m_month, 0);
        TS_ASSERT_EQUALS(pt.m_day, 99);
        TS_ASSERT_EQUALS(pt.m_hour, 0);
        TS_ASSERT_EQUALS(pt.m_minute, 0);
        TS_ASSERT_EQUALS(pt.m_second, 0);
        TS_ASSERT_EQUALS(pt.m_millisecond, 9);
        TS_ASSERT_EQUALS(pt.m_weekday, 0);
    }
}

/** Test comparisons. */
void
TestSysDuration::testComparison()
{
    using afl::sys::Duration;

    const Duration a(100);
    const Duration b(100);
    const Duration c(200);

    TS_ASSERT( (a == b));
    TS_ASSERT(!(a == c));

    TS_ASSERT(!(a != b));
    TS_ASSERT( (a != c));

    TS_ASSERT( (a <= b));
    TS_ASSERT( (a <= c));

    TS_ASSERT( (a >= b));
    TS_ASSERT(!(a >= c));

    TS_ASSERT(!(a < b));
    TS_ASSERT( (a < c));

    TS_ASSERT(!(a > b));
    TS_ASSERT(!(a > c));
}

/** Test arithmetic. */
void
TestSysDuration::testArithmetic()
{
    using afl::sys::Duration;

    TS_ASSERT_EQUALS(Duration(1) + Duration(2), Duration(3));
    TS_ASSERT_EQUALS(Duration(99) - Duration(88), Duration(11));
    TS_ASSERT_EQUALS(Duration(1) - Duration(2), Duration(-1));

    Duration d(9);
    TS_ASSERT_EQUALS(d, Duration(9));
    d += Duration::fromSeconds(3);
    TS_ASSERT_EQUALS(d, Duration(3009));
    d -= Duration::fromSeconds(2);
    TS_ASSERT_EQUALS(d, Duration(1009));
}

/** Test toTimeout. */
void
TestSysDuration::testTimeout()
{
    using afl::sys::Duration;

    // Normal cases
    TS_ASSERT_EQUALS(Duration(0).toTimeout(), 0U);
    TS_ASSERT_EQUALS(Duration::fromSeconds(2).toTimeout(), 2000U);
    TS_ASSERT_EQUALS(Duration::fromMinutes(1).toTimeout(), 60000U);

    // Underflow case
    TS_ASSERT_EQUALS(Duration::fromSeconds(-1).toTimeout(), 0U);
    TS_ASSERT_EQUALS(Duration::fromHours(-1).toTimeout(), 0U);

    // Overflow cases
    TS_ASSERT(Duration(0x100000000LL).toTimeout() < afl::sys::INFINITE_TIMEOUT); 
    TS_ASSERT(Duration::fromHours(100 * 24).toTimeout() < afl::sys::INFINITE_TIMEOUT);

    TS_ASSERT_EQUALS(Duration::fromHours(100 * 24).toTimeout(), Duration::fromHours(101 * 24).toTimeout());
}
