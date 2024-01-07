/**
  *  \file test/afl/sys/timetest.cpp
  *  \brief Test for afl::sys::Time
  */

#include "afl/sys/time.hpp"

#include "afl/sys/duration.hpp"
#include "afl/sys/parsedtime.hpp"
#include "afl/test/testrunner.hpp"

using afl::sys::Duration;
using afl::sys::ParsedTime;
using afl::sys::Time;

/*
 *  Construction and deconstruction
 */
AFL_TEST("afl.sys.Time:construct", a)
{
    // Default
    a.checkEqual("01", Time().getRepresentation(), 0);
    a.check("02", !Time().isValid());

    // Value
    a.checkEqual("11", Time(99).getRepresentation(), 99);
    a.check("12", Time(99).isValid());
}

// From parsed time
AFL_TEST("afl.sys.Time:construct:ParsedTime", a)
{
    static const afl::sys::ParsedTime pt  = { 1990, 3, 7, 13, 32, 32, 500, 0 };
    a.check("01. getRepresentation", Time(pt, Time::LocalTime).getRepresentation() > 0);
    a.check("02. getRepresentation", Time(pt, Time::UniversalTime).getRepresentation() > 0);
    a.check("03. isValid", Time(pt, Time::LocalTime).isValid());
    a.check("04. isValid", Time(pt, Time::UniversalTime).isValid());
    a.checkEqual("05. getUnixTime", Time(pt, Time::UniversalTime).getUnixTime(), 636816752);

    // Compare with Unix time. Since we have 500 ms, we're between two unix times
    a.check("11. fromUnixTime", Time::fromUnixTime(636816752) < Time(pt, Time::UniversalTime));
    a.check("12. fromUnixTime", Time::fromUnixTime(636816753) > Time(pt, Time::UniversalTime));

    // Compare with a time five seconds later
    // (we assume nobody does a DST switch at 13:32:32.)
    static const afl::sys::ParsedTime pt2 = { 1990, 3, 7, 13, 32, 37, 500, 0 };
    a.check("21. op<", Time(pt, Time::LocalTime) < Time(pt2, Time::LocalTime));
    a.check("22. op<", Time(pt, Time::UniversalTime) < Time(pt2, Time::UniversalTime));
    a.checkEqual("23. getUnixTime", Time(pt2, Time::UniversalTime).getUnixTime(), 636816757);

    // Unpack
    afl::sys::ParsedTime pt3;
    Time(pt2, Time::LocalTime).unpack(pt3, Time::LocalTime);
    a.checkEqual("31. year",    pt3.m_year, 1990);
    a.checkEqual("32. month",   pt3.m_month, 3);
    a.checkEqual("33. day",     pt3.m_day, 7);
    a.checkEqual("34. hour",    pt3.m_hour, 13);
    a.checkEqual("35. minute",  pt3.m_minute, 32);
    a.checkEqual("36. second",  pt3.m_second, 37);
    a.checkEqual("37. ms",      pt3.m_millisecond, 500);
    a.checkEqual("38. weekday", pt3.m_weekday, 3);          // March 7 1990 was a Wednesday

    Time(pt2, Time::UniversalTime).unpack(pt3, Time::UniversalTime);
    a.checkEqual("41. year",    pt3.m_year, 1990);
    a.checkEqual("42. month",   pt3.m_month, 3);
    a.checkEqual("43. day",     pt3.m_day, 7);
    a.checkEqual("44. hour",    pt3.m_hour, 13);
    a.checkEqual("45. minute",  pt3.m_minute, 32);
    a.checkEqual("46. second",  pt3.m_second, 37);
    a.checkEqual("47. ms",      pt3.m_millisecond, 500);
    a.checkEqual("48. weekday", pt3.m_weekday, 3);          // March 7 1990 was a Wednesday
}

// Time in January
AFL_TEST("afl.sys.Time:construct:local-jan", a)
{
    static const ParsedTime pt = { 1990, 1, 1, 10, 0, 0, 500, 0 };
    ParsedTime pt2;
    Time(pt, Time::LocalTime).unpack(pt2, Time::LocalTime);
    a.checkEqual("01. year",   pt.m_year,   pt2.m_year);
    a.checkEqual("02. month",  pt.m_month,  pt2.m_month);
    a.checkEqual("03. day",    pt.m_day,    pt2.m_day);
    a.checkEqual("04. hour",   pt.m_hour,   pt2.m_hour);
    a.checkEqual("05. minute", pt.m_minute, pt2.m_minute);
}

// Time in July
AFL_TEST("afl.sys.Time:construct:local-jul", a)
{
    static const ParsedTime pt = { 1990, 7, 1, 10, 0, 0, 500, 0 };
    ParsedTime pt2;
    Time(pt, Time::LocalTime).unpack(pt2, Time::LocalTime);
    a.checkEqual("01. year",   pt.m_year,   pt2.m_year);
    a.checkEqual("02. month",  pt.m_month,  pt2.m_month);
    a.checkEqual("03. day",    pt.m_day,    pt2.m_day);
    a.checkEqual("04. hour",   pt.m_hour,   pt2.m_hour);
    a.checkEqual("05. minute", pt.m_minute, pt2.m_minute);
}

/** Test comparisons. */
AFL_TEST("afl.sys.Time:comparison", a)
{
    const Time ta(100);
    const Time tb(100);
    const Time tc(200);

    a.check("01. eq",  (ta == tb));
    a.check("02. eq", !(ta == tc));

    a.check("11. ne", !(ta != tb));
    a.check("12. ne",  (ta != tc));

    a.check("21. le",  (ta <= tb));
    a.check("22. le",  (ta <= tc));

    a.check("31. ge",  (ta >= tb));
    a.check("32. ge", !(ta >= tc));

    a.check("41. lt", !(ta < tb));
    a.check("42. lt",  (ta < tc));

    a.check("51. gt", !(ta > tb));
    a.check("52. gt", !(ta > tc));
}

/** Test arithmetic. */
AFL_TEST("afl.sys.Time:arithmetic", a)
{
    a.check("01. plus",  Time(99) + Duration(11) == Time(110));
    a.check("02. minus", Time(99) - Duration(11) == Time(88));
    a.check("03. minus", Time(99) - Time(11)     == Duration(88));

    Time t(99);
    t += Duration(22);
    a.check("11. op+=", t == Time(121));
    t -= Duration(21);
    a.check("12. op-=", t ==Time(100));
}

/** Test current time access. */
AFL_TEST("afl.sys.Time:getCurrentTime", a)
{
    Time nowTime      = Time::getCurrentTime();
    uint32_t nowTicks = Time::getTickCounter();

    Time thenTime;
    uint32_t thenTicks;

    // Test that both the tick counter and the current time advance at a similar pace.
    // Since timing totally depends on the environment, we cannot be too picky.
    // Problems of this test:
    // - a system that reports only whole seconds might fail the [200,800] test
    // - a system where the tick counter does not increase might loop forever
    while (1) {
        thenTime  = Time::getCurrentTime();
        thenTicks = Time::getTickCounter();

        if (thenTicks - nowTicks > 500) {
            break;
        }
    }

    a.check("min ticks", thenTime - nowTime > afl::sys::Duration(200));
    a.check("max ticks", thenTime - nowTime < afl::sys::Duration(800));

    // Parse the time. This may fail on systems with no clock.
    afl::sys::ParsedTime pt;
    nowTime.unpack(pt, Time::LocalTime);
    a.check("year",        pt.m_year > 2000);
    a.check("min month",   pt.m_month >= 1);
    a.check("max month",   pt.m_month <= 12);
    a.check("min day",     pt.m_day >= 1);
    a.check("max day",     pt.m_day <= 31);
    a.check("max hour",    pt.m_hour < 24);
    a.check("max minute",  pt.m_minute < 60);
    a.check("max second",  pt.m_second < 62);
    a.check("max ms",      pt.m_millisecond < 1000);
    a.check("max weekday", pt.m_weekday < 7);
}

/** Test toString. */
AFL_TEST("afl.sys.Time:toString", a)
{
    // Sun Jul 31 23:20:42 CEST 2016
    afl::sys::Time t = afl::sys::Time::fromUnixTime(1470000042);

    // Fetch times.
    String_t univFull = t.toString(afl::sys::Time::UniversalTime, afl::sys::Time::FullFormat);
    String_t univDate = t.toString(afl::sys::Time::UniversalTime, afl::sys::Time::DateFormat);
    String_t univTime = t.toString(afl::sys::Time::UniversalTime, afl::sys::Time::TimeFormat);

    // Validate.
    // These are locale and operating-system dependant.
    // However, we're pretty sure that dates contain "16", and times contain "42" (=seconds).
    a.check("01. univFull date",   univFull.find("16") != String_t::npos);
    a.check("02. univDate date",   univDate.find("16") != String_t::npos);
    a.check("03. univFull second", univFull.find("42") != String_t::npos);
    a.check("04. univTime second", univTime.find("42") != String_t::npos);

    // Same thing with local time
    String_t localFull = t.toString(afl::sys::Time::LocalTime, afl::sys::Time::FullFormat);
    String_t localDate = t.toString(afl::sys::Time::LocalTime, afl::sys::Time::DateFormat);
    String_t localTime = t.toString(afl::sys::Time::LocalTime, afl::sys::Time::TimeFormat);

    a.check("11. localFull date",   localFull.find("16") != String_t::npos);
    a.check("12. localDate date",   localDate.find("16") != String_t::npos);
    a.check("13. localFull second", localFull.find("42") != String_t::npos);
    a.check("14. localTime second", localTime.find("42") != String_t::npos);
}
