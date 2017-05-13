/**
  *  \file u/t_sys_parsedtime.cpp
  *  \brief Test for afl::sys::ParsedTime
  */

#include "afl/sys/parsedtime.hpp"
#include "u/t_sys.hpp"

/** Simple test.
    This is just a structure, so all we can test is that it can be defined
    and its members used. */
void
TestSysParsedTime::testIt()
{
    afl::sys::ParsedTime pt;
    pt.m_year        = 2050;
    pt.m_month       = 12;
    pt.m_day         = 31;
    pt.m_hour        = 23;
    pt.m_minute      = 59;
    pt.m_second      = 60;           // leaf seconds FTW!
    pt.m_millisecond = 999;
    pt.m_weekday     = 6;

    TS_ASSERT_EQUALS(pt.m_year, 2050U);
    TS_ASSERT_EQUALS(pt.m_month, 12U);
    TS_ASSERT_EQUALS(pt.m_day, 31U);
    TS_ASSERT_EQUALS(pt.m_hour, 23U);
    TS_ASSERT_EQUALS(pt.m_minute, 59U);
    TS_ASSERT_EQUALS(pt.m_second, 60U);
    TS_ASSERT_EQUALS(pt.m_millisecond, 999U);
    TS_ASSERT_EQUALS(pt.m_weekday, 6U);
}

/** Test formatting. */
void
TestSysParsedTime::testFormat()
{
    afl::sys::ParsedTime pt;
    pt.m_year        = 2050;
    pt.m_month       = 12;
    pt.m_day         = 31;
    pt.m_hour        = 23;
    pt.m_minute      = 59;
    pt.m_second      = 60;
    pt.m_millisecond = 999;
    pt.m_weekday     = 6;

    // Simple cases
    TS_ASSERT_EQUALS(pt.format(0), "");
    TS_ASSERT_EQUALS(pt.format(""), "");
    TS_ASSERT_EQUALS(pt.format("hi mom"), "hi mom");
    TS_ASSERT_EQUALS(pt.format("%"), "");
    TS_ASSERT_EQUALS(pt.format("%E"), "");
    TS_ASSERT_EQUALS(pt.format("%O"), "");

    // Single placeholders
    TS_ASSERT_EQUALS(pt.format("%a"), "Sat");
    TS_ASSERT_EQUALS(pt.format("%A"), "Saturday");
    TS_ASSERT_EQUALS(pt.format("%b"), "Dec");
    TS_ASSERT_EQUALS(pt.format("%B"), "December");
    TS_ASSERT_EQUALS(pt.format("%c"), "2050-12-31 23:59:60");
    TS_ASSERT_EQUALS(pt.format("%C"), "20");
    TS_ASSERT_EQUALS(pt.format("%d"), "31");
    TS_ASSERT_EQUALS(pt.format("%D"), "12/31/50");
    TS_ASSERT_EQUALS(pt.format("%e"), "31");
    TS_ASSERT_EQUALS(pt.format("%F"), "2050-12-31");
    TS_ASSERT_EQUALS(pt.format("%h"), "Dec");
    TS_ASSERT_EQUALS(pt.format("%H"), "23");
    TS_ASSERT_EQUALS(pt.format("%I"), "11");
    TS_ASSERT_EQUALS(pt.format("%m"), "12");
    TS_ASSERT_EQUALS(pt.format("%M"), "59");
    TS_ASSERT_EQUALS(pt.format("%n"), "\n");
    TS_ASSERT_EQUALS(pt.format("%p"), "PM");
    TS_ASSERT_EQUALS(pt.format("%r"), "11:59:60 PM");
    TS_ASSERT_EQUALS(pt.format("%R"), "23:59");
    TS_ASSERT_EQUALS(pt.format("%S"), "60");
    TS_ASSERT_EQUALS(pt.format("%t"), "\t");
    TS_ASSERT_EQUALS(pt.format("%T"), "23:59:60");
    TS_ASSERT_EQUALS(pt.format("%u"), "6");
    TS_ASSERT_EQUALS(pt.format("%w"), "6");
    TS_ASSERT_EQUALS(pt.format("%x"), "2050-12-31");
    TS_ASSERT_EQUALS(pt.format("%X"), "23:59:60");
    TS_ASSERT_EQUALS(pt.format("%y"), "50");
    TS_ASSERT_EQUALS(pt.format("%Y"), "2050");
    TS_ASSERT_EQUALS(pt.format("%z"), "");
    TS_ASSERT_EQUALS(pt.format("%Z"), "");
    TS_ASSERT_EQUALS(pt.format("%%"), "%");
    TS_ASSERT_EQUALS(pt.format("%_"), "_");

    pt.m_year        = 1912;
    pt.m_month       = 3;
    pt.m_day         = 5;       // one-digit to test %d vs %e
    pt.m_hour        = 11;
    pt.m_minute      = 40;
    pt.m_second      = 20;
    pt.m_millisecond = 999;
    pt.m_weekday     = 0;

    // Simple cases
    TS_ASSERT_EQUALS(pt.format(0), "");
    TS_ASSERT_EQUALS(pt.format(""), "");
    TS_ASSERT_EQUALS(pt.format("hi mom"), "hi mom");

    // Single placeholders
    TS_ASSERT_EQUALS(pt.format("%a"), "Sun");
    TS_ASSERT_EQUALS(pt.format("%A"), "Sunday");
    TS_ASSERT_EQUALS(pt.format("%b"), "Mar");
    TS_ASSERT_EQUALS(pt.format("%B"), "March");
    TS_ASSERT_EQUALS(pt.format("%c"), "1912-03-05 11:40:20");
    TS_ASSERT_EQUALS(pt.format("%C"), "19");
    TS_ASSERT_EQUALS(pt.format("%d"), "05");
    TS_ASSERT_EQUALS(pt.format("%D"), "03/05/12");
    TS_ASSERT_EQUALS(pt.format("%e"), " 5");
    TS_ASSERT_EQUALS(pt.format("%F"), "1912-03-05");
    TS_ASSERT_EQUALS(pt.format("%h"), "Mar");
    TS_ASSERT_EQUALS(pt.format("%H"), "11");
    TS_ASSERT_EQUALS(pt.format("%I"), "11");
    TS_ASSERT_EQUALS(pt.format("%m"), "03");
    TS_ASSERT_EQUALS(pt.format("%M"), "40");
    TS_ASSERT_EQUALS(pt.format("%p"), "AM");
    TS_ASSERT_EQUALS(pt.format("%r"), "11:40:20 AM");
    TS_ASSERT_EQUALS(pt.format("%R"), "11:40");
    TS_ASSERT_EQUALS(pt.format("%S"), "20");
    TS_ASSERT_EQUALS(pt.format("%T"), "11:40:20");
    TS_ASSERT_EQUALS(pt.format("%u"), "7");
    TS_ASSERT_EQUALS(pt.format("%w"), "0");
    TS_ASSERT_EQUALS(pt.format("%x"), "1912-03-05");
    TS_ASSERT_EQUALS(pt.format("%X"), "11:40:20");
    TS_ASSERT_EQUALS(pt.format("%y"), "12");
    TS_ASSERT_EQUALS(pt.format("%Y"), "1912");

    // Modifiers
    TS_ASSERT_EQUALS(pt.format("%Ey"), "12");
    TS_ASSERT_EQUALS(pt.format("%Oy"), "12");
    TS_ASSERT_EQUALS(pt.format("%O%"), "%");
}

/** Test month formatting. */
void
TestSysParsedTime::testMonths()
{
    afl::sys::ParsedTime pt = afl::sys::ParsedTime();

    // In-range
    pt.m_month = 1;
    TS_ASSERT_EQUALS(pt.format("%b|%B"), "Jan|January");
    pt.m_month = 2;
    TS_ASSERT_EQUALS(pt.format("%b|%B"), "Feb|February");
    pt.m_month = 3;
    TS_ASSERT_EQUALS(pt.format("%b|%B"), "Mar|March");
    pt.m_month = 4;
    TS_ASSERT_EQUALS(pt.format("%b|%B"), "Apr|April");
    pt.m_month = 5;
    TS_ASSERT_EQUALS(pt.format("%b|%B"), "May|May");
    pt.m_month = 6;
    TS_ASSERT_EQUALS(pt.format("%b|%B"), "Jun|June");
    pt.m_month = 7;
    TS_ASSERT_EQUALS(pt.format("%b|%B"), "Jul|July");
    pt.m_month = 8;
    TS_ASSERT_EQUALS(pt.format("%b|%B"), "Aug|August");
    pt.m_month = 9;
    TS_ASSERT_EQUALS(pt.format("%b|%B"), "Sep|September");
    pt.m_month = 10;
    TS_ASSERT_EQUALS(pt.format("%b|%B"), "Oct|October");
    pt.m_month = 11;
    TS_ASSERT_EQUALS(pt.format("%b|%B"), "Nov|November");
    pt.m_month = 12;
    TS_ASSERT_EQUALS(pt.format("%b|%B"), "Dec|December");

    // Out-of-range
    pt.m_month = 0;
    TS_ASSERT_EQUALS(pt.format("%b|%B"), "|");
    pt.m_month = 13;
    TS_ASSERT_EQUALS(pt.format("%b|%B"), "|");
    pt.m_month = 1000;
    TS_ASSERT_EQUALS(pt.format("%b|%B"), "|");
}

/** Test weekday formatting. */
void
TestSysParsedTime::testWeekdays()
{
    afl::sys::ParsedTime pt = afl::sys::ParsedTime();

    // In-range
    pt.m_weekday = 0;
    TS_ASSERT_EQUALS(pt.format("%a|%A|%u|%w"), "Sun|Sunday|7|0");
    pt.m_weekday = 1;
    TS_ASSERT_EQUALS(pt.format("%a|%A|%u|%w"), "Mon|Monday|1|1");
    pt.m_weekday = 2;
    TS_ASSERT_EQUALS(pt.format("%a|%A|%u|%w"), "Tue|Tuesday|2|2");
    pt.m_weekday = 3;
    TS_ASSERT_EQUALS(pt.format("%a|%A|%u|%w"), "Wed|Wednesday|3|3");
    pt.m_weekday = 4;
    TS_ASSERT_EQUALS(pt.format("%a|%A|%u|%w"), "Thu|Thursday|4|4");
    pt.m_weekday = 5;
    TS_ASSERT_EQUALS(pt.format("%a|%A|%u|%w"), "Fri|Friday|5|5");
    pt.m_weekday = 6;
    TS_ASSERT_EQUALS(pt.format("%a|%A|%u|%w"), "Sat|Saturday|6|6");

    // Out-of-range
    pt.m_weekday = 8;
    TS_ASSERT_EQUALS(pt.format("%a|%A|%u|%w"), "|||8");
}

/** Test hour formatting. */
void
TestSysParsedTime::testHours()
{
    afl::sys::ParsedTime pt = afl::sys::ParsedTime();

    pt.m_hour = 0;
    TS_ASSERT_EQUALS(pt.format("%H|%I|%p"), "00|12|AM");
    pt.m_hour = 1;
    TS_ASSERT_EQUALS(pt.format("%H|%I|%p"), "01|01|AM");
    pt.m_hour = 2;
    TS_ASSERT_EQUALS(pt.format("%H|%I|%p"), "02|02|AM");
    pt.m_hour = 3;
    TS_ASSERT_EQUALS(pt.format("%H|%I|%p"), "03|03|AM");
    pt.m_hour = 4;
    TS_ASSERT_EQUALS(pt.format("%H|%I|%p"), "04|04|AM");
    pt.m_hour = 5;
    TS_ASSERT_EQUALS(pt.format("%H|%I|%p"), "05|05|AM");
    pt.m_hour = 6;
    TS_ASSERT_EQUALS(pt.format("%H|%I|%p"), "06|06|AM");
    pt.m_hour = 7;
    TS_ASSERT_EQUALS(pt.format("%H|%I|%p"), "07|07|AM");
    pt.m_hour = 8;
    TS_ASSERT_EQUALS(pt.format("%H|%I|%p"), "08|08|AM");
    pt.m_hour = 9;
    TS_ASSERT_EQUALS(pt.format("%H|%I|%p"), "09|09|AM");
    pt.m_hour = 10;
    TS_ASSERT_EQUALS(pt.format("%H|%I|%p"), "10|10|AM");
    pt.m_hour = 11;
    TS_ASSERT_EQUALS(pt.format("%H|%I|%p"), "11|11|AM");
    pt.m_hour = 12;
    TS_ASSERT_EQUALS(pt.format("%H|%I|%p"), "12|12|PM");
    pt.m_hour = 13;
    TS_ASSERT_EQUALS(pt.format("%H|%I|%p"), "13|01|PM");
    pt.m_hour = 14;
    TS_ASSERT_EQUALS(pt.format("%H|%I|%p"), "14|02|PM");
    pt.m_hour = 15;
    TS_ASSERT_EQUALS(pt.format("%H|%I|%p"), "15|03|PM");
    pt.m_hour = 16;
    TS_ASSERT_EQUALS(pt.format("%H|%I|%p"), "16|04|PM");
    pt.m_hour = 17;
    TS_ASSERT_EQUALS(pt.format("%H|%I|%p"), "17|05|PM");
    pt.m_hour = 18;
    TS_ASSERT_EQUALS(pt.format("%H|%I|%p"), "18|06|PM");
    pt.m_hour = 19;
    TS_ASSERT_EQUALS(pt.format("%H|%I|%p"), "19|07|PM");
    pt.m_hour = 20;
    TS_ASSERT_EQUALS(pt.format("%H|%I|%p"), "20|08|PM");
    pt.m_hour = 21;
    TS_ASSERT_EQUALS(pt.format("%H|%I|%p"), "21|09|PM");
    pt.m_hour = 22;
    TS_ASSERT_EQUALS(pt.format("%H|%I|%p"), "22|10|PM");
    pt.m_hour = 23;
    TS_ASSERT_EQUALS(pt.format("%H|%I|%p"), "23|11|PM");
}

