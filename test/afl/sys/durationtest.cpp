/**
  *  \file test/afl/sys/durationtest.cpp
  *  \brief Test for afl::sys::Duration
  */

#include "afl/sys/duration.hpp"

#include "afl/sys/parsedtime.hpp"
#include "afl/test/testrunner.hpp"

using afl::sys::Duration;

/*
 *  Construction and deconstruction
 */
AFL_TEST("afl.sys.Duration:construct", a)
{
    // 0
    a.checkEqual("01", Duration().getMilliseconds(), 0);
    a.check("02", Duration().isValid());

    // 999
    a.checkEqual("03", Duration(999).getMilliseconds(), 999);
    a.check("04", Duration(999).isValid());

    // Negative
    a.checkEqual("05", Duration(-42).getMilliseconds(), -42);
    a.check("06", !Duration(-42).isValid());

    // Units
    a.checkEqual("07", Duration::fromSeconds(3).getMilliseconds(), 3000);
    a.checkEqual("08", Duration::fromMinutes(7).getMilliseconds(), 7LL*60*1000);
    a.checkEqual("09", Duration::fromHours(2).getMilliseconds(), 2LL*60*60*1000);
}

// ParsedTime
AFL_TEST("afl.sys.Duration:construct:parsedtime:1", a)
{
    afl::sys::ParsedTime pt = afl::sys::ParsedTime();
    pt.m_minute = 2;
    a.checkEqual("", Duration(pt).getMilliseconds(), 2LL*60*1000);
}

AFL_TEST("afl.sys.Duration:construct:parsedtime:2", a)
{
    afl::sys::ParsedTime pt = afl::sys::ParsedTime();
    pt.m_day = 99;
    pt.m_millisecond = 9;
    a.checkEqual("", Duration(pt).getMilliseconds(), 8553600009LL);
}

AFL_TEST("afl.sys.Duration:construct:parsedtime:3", a)
{
    afl::sys::ParsedTime pt = afl::sys::ParsedTime();
    pt.m_minute = 100;
    a.checkEqual("", Duration(pt).getMilliseconds(), 6000000LL);
}

AFL_TEST("afl.sys.Duration:unpack", a)
{
    afl::sys::ParsedTime pt;
    Duration(8553600009LL).unpack(pt);
    a.checkEqual("year",    pt.m_year, 0);
    a.checkEqual("month",   pt.m_month, 0);
    a.checkEqual("day",     pt.m_day, 99);
    a.checkEqual("hour",    pt.m_hour, 0);
    a.checkEqual("minute",  pt.m_minute, 0);
    a.checkEqual("second",  pt.m_second, 0);
    a.checkEqual("ms",      pt.m_millisecond, 9);
    a.checkEqual("weekday", pt.m_weekday, 0);
}

/** Test comparisons. */
AFL_TEST("afl.sys.Duration:comparison", a)
{
    const Duration da(100);
    const Duration db(100);
    const Duration dc(200);

    a.check("eq 1",  (da == db));
    a.check("eq 2", !(da == dc));

    a.check("ne 1", !(da != db));
    a.check("ne 2",  (da != dc));

    a.check("le 1",  (da <= db));
    a.check("le 2",  (da <= dc));

    a.check("ge 1",  (da >= db));
    a.check("ge 2", !(da >= dc));

    a.check("lt 1", !(da < db));
    a.check("lt 2",  (da < dc));

    a.check("gt 1", !(da > db));
    a.check("gt 2", !(da > dc));
}

/** Test arithmetic. */
AFL_TEST("afl.sys.Duration:arithmetic", a)
{
    a.check("01. op+", Duration(1)  + Duration(2)  == Duration(3));
    a.check("02. op-", Duration(99) - Duration(88) == Duration(11));
    a.check("03. op-", Duration(1)  - Duration(2)  == Duration(-1));

    Duration d(9);
    a.check("11. init", d == Duration(9));
    d += Duration::fromSeconds(3);
    a.check("12. op+=", d == Duration(3009));
    d -= Duration::fromSeconds(2);
    a.check("13. op-=", d == Duration(1009));
}

/** Test toTimeout. */
AFL_TEST("afl.sys.Duration:toTimeout", a)
{
    // Normal cases
    a.checkEqual("01", Duration(0).toTimeout(), 0U);
    a.checkEqual("02", Duration::fromSeconds(2).toTimeout(), 2000U);
    a.checkEqual("03", Duration::fromMinutes(1).toTimeout(), 60000U);

    // Underflow case
    a.checkEqual("11", Duration::fromSeconds(-1).toTimeout(), 0U);
    a.checkEqual("12", Duration::fromHours(-1).toTimeout(), 0U);

    // Overflow cases
    a.check("21", Duration(0x100000000LL).toTimeout() < afl::sys::INFINITE_TIMEOUT);
    a.check("22", Duration::fromHours(100 * 24).toTimeout() < afl::sys::INFINITE_TIMEOUT);

    a.checkEqual("23", Duration::fromHours(100 * 24).toTimeout(), Duration::fromHours(101 * 24).toTimeout());
}
