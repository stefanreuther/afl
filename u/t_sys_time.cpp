/**
  *  \file u/t_sys_time.cpp
  *  \brief Test for afl::sys::Time
  */

#include "afl/sys/time.hpp"

#include "u/t_sys.hpp"
#include "afl/sys/parsedtime.hpp"

/** Test construction and deconstruction. */
void
TestSysTime::testConstruction()
{
    using afl::sys::Time;

    // Default
    TS_ASSERT_EQUALS(Time().getRepresentation(), 0);
    TS_ASSERT(!Time().isValid());

    // Value
    TS_ASSERT_EQUALS(Time(99).getRepresentation(), 99);
    TS_ASSERT(Time(99).isValid());

    // From parsed time
    {
        static const afl::sys::ParsedTime pt  = { 1990, 3, 7, 13, 32, 32, 500, 0 };
        TS_ASSERT(Time(pt, Time::LocalTime).getRepresentation() > 0);
        TS_ASSERT(Time(pt, Time::UniversalTime).getRepresentation() > 0);
        TS_ASSERT(Time(pt, Time::LocalTime).isValid());
        TS_ASSERT(Time(pt, Time::UniversalTime).isValid());
        TS_ASSERT_EQUALS(Time(pt, Time::UniversalTime).getUnixTime(), 636816752);

        // Compare with Unix time. Since we have 500 ms, we're between two unix times
        TS_ASSERT(Time::fromUnixTime(636816752) < Time(pt, Time::UniversalTime));
        TS_ASSERT(Time::fromUnixTime(636816753) > Time(pt, Time::UniversalTime));

        // Compare with a time five seconds later
        // (we assume nobody does a DST switch at 13:32:32.)
        static const afl::sys::ParsedTime pt2 = { 1990, 3, 7, 13, 32, 37, 500, 0 };
        TS_ASSERT(Time(pt, Time::LocalTime) < Time(pt2, Time::LocalTime));
        TS_ASSERT(Time(pt, Time::UniversalTime) < Time(pt2, Time::UniversalTime));
        TS_ASSERT_EQUALS(Time(pt2, Time::UniversalTime).getUnixTime(), 636816757);

        // Unpack
        afl::sys::ParsedTime pt3;
        Time(pt2, Time::LocalTime).unpack(pt3, Time::LocalTime);
        TS_ASSERT_EQUALS(pt3.m_year, 1990);
        TS_ASSERT_EQUALS(pt3.m_month, 3);
        TS_ASSERT_EQUALS(pt3.m_day, 7);
        TS_ASSERT_EQUALS(pt3.m_hour, 13);
        TS_ASSERT_EQUALS(pt3.m_minute, 32);
        TS_ASSERT_EQUALS(pt3.m_second, 37);
        TS_ASSERT_EQUALS(pt3.m_millisecond, 500);
        TS_ASSERT_EQUALS(pt3.m_weekday, 3);          // March 7 1990 was a Wednesday

        Time(pt2, Time::UniversalTime).unpack(pt3, Time::UniversalTime);
        TS_ASSERT_EQUALS(pt3.m_year, 1990);
        TS_ASSERT_EQUALS(pt3.m_month, 3);
        TS_ASSERT_EQUALS(pt3.m_day, 7);
        TS_ASSERT_EQUALS(pt3.m_hour, 13);
        TS_ASSERT_EQUALS(pt3.m_minute, 32);
        TS_ASSERT_EQUALS(pt3.m_second, 37);
        TS_ASSERT_EQUALS(pt3.m_millisecond, 500);
        TS_ASSERT_EQUALS(pt3.m_weekday, 3);          // March 7 1990 was a Wednesday
    }
}

/** Test construction and deconstruction from local time. */
void
TestSysTime::testConstructionLocal()
{
    using afl::sys::Time;
    using afl::sys::ParsedTime;

    // Time in January
    {
        static const ParsedTime pt = { 1990, 1, 1, 10, 0, 0, 500, 0 };
        ParsedTime pt2;
        Time(pt, Time::LocalTime).unpack(pt2, Time::LocalTime);
        TS_ASSERT_EQUALS(pt.m_year,   pt2.m_year);
        TS_ASSERT_EQUALS(pt.m_month,  pt2.m_month);
        TS_ASSERT_EQUALS(pt.m_day,    pt2.m_day);
        TS_ASSERT_EQUALS(pt.m_hour,   pt2.m_hour);
        TS_ASSERT_EQUALS(pt.m_minute, pt2.m_minute);
    }

    // Time in July
    {
        static const ParsedTime pt = { 1990, 7, 1, 10, 0, 0, 500, 0 };
        ParsedTime pt2;
        Time(pt, Time::LocalTime).unpack(pt2, Time::LocalTime);
        TS_ASSERT_EQUALS(pt.m_year,   pt2.m_year);
        TS_ASSERT_EQUALS(pt.m_month,  pt2.m_month);
        TS_ASSERT_EQUALS(pt.m_day,    pt2.m_day);
        TS_ASSERT_EQUALS(pt.m_hour,   pt2.m_hour);
        TS_ASSERT_EQUALS(pt.m_minute, pt2.m_minute);
    }
}

/** Test comparisons. */
void
TestSysTime::testComparison()
{
    using afl::sys::Time;

    const Time a(100);
    const Time b(100);
    const Time c(200);

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
TestSysTime::testArithmetic()
{
    using afl::sys::Time;
    using afl::sys::Duration;

    TS_ASSERT_EQUALS(Time(99) + Duration(11), Time(110));
    TS_ASSERT_EQUALS(Time(99) - Duration(11), Time(88));
    TS_ASSERT_EQUALS(Time(99) - Time(11), Duration(88));

    Time t(99);
    t += Duration(22);
    TS_ASSERT_EQUALS(t, Time(121));
    t -= Duration(21);
    TS_ASSERT_EQUALS(t, Time(100));
}

/** Test current time access. */
void
TestSysTime::testCurrent()
{
    using afl::sys::Time;

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

    TS_ASSERT(thenTime - nowTime > afl::sys::Duration(200));
    TS_ASSERT(thenTime - nowTime < afl::sys::Duration(800));

    // Parse the time. This may fail on systems with no clock.
    afl::sys::ParsedTime pt;
    nowTime.unpack(pt, Time::LocalTime);
    TS_ASSERT(pt.m_year > 2000);
    TS_ASSERT(pt.m_month >= 1);
    TS_ASSERT(pt.m_month <= 12);
    TS_ASSERT(pt.m_day >= 1);
    TS_ASSERT(pt.m_day <= 31);
    TS_ASSERT(pt.m_hour < 24);
    TS_ASSERT(pt.m_minute < 60);
    TS_ASSERT(pt.m_second < 62);
    TS_ASSERT(pt.m_millisecond < 1000);
    TS_ASSERT(pt.m_weekday < 7);
}

/** Test toString. */
void
TestSysTime::testToString()
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
    TSM_ASSERT(univFull, univFull.find("16") != String_t::npos);
    TSM_ASSERT(univDate, univDate.find("16") != String_t::npos);
    TSM_ASSERT(univFull, univFull.find("42") != String_t::npos);
    TSM_ASSERT(univTime, univTime.find("42") != String_t::npos);

    // Same thing with local time
    String_t localFull = t.toString(afl::sys::Time::LocalTime, afl::sys::Time::FullFormat);
    String_t localDate = t.toString(afl::sys::Time::LocalTime, afl::sys::Time::DateFormat);
    String_t localTime = t.toString(afl::sys::Time::LocalTime, afl::sys::Time::TimeFormat);

    TSM_ASSERT(localFull, localFull.find("16") != String_t::npos);
    TSM_ASSERT(localDate, localDate.find("16") != String_t::npos);
    TSM_ASSERT(localFull, localFull.find("42") != String_t::npos);
    TSM_ASSERT(localTime, localTime.find("42") != String_t::npos);
}

