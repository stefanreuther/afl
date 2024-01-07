/**
  *  \file test/afl/sys/parsedtimetest.cpp
  *  \brief Test for afl::sys::ParsedTime
  */

#include "afl/sys/parsedtime.hpp"
#include "afl/test/testrunner.hpp"

/** Simple test.
    This is just a structure, so all we can test is that it can be defined
    and its members used. */
AFL_TEST("afl.sys.ParsedTime:struct", a)
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

    a.checkEqual("year",    pt.m_year, 2050U);
    a.checkEqual("month",   pt.m_month, 12U);
    a.checkEqual("day",     pt.m_day, 31U);
    a.checkEqual("hour",    pt.m_hour, 23U);
    a.checkEqual("minute",  pt.m_minute, 59U);
    a.checkEqual("second",  pt.m_second, 60U);
    a.checkEqual("ms",      pt.m_millisecond, 999U);
    a.checkEqual("weekday", pt.m_weekday, 6U);
}

/** Test formatting. */
AFL_TEST("afl.sys.ParsedTime:format", a)
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
    a.checkEqual("01", pt.format(0), "");
    a.checkEqual("02", pt.format(""), "");
    a.checkEqual("03", pt.format("hi mom"), "hi mom");
    a.checkEqual("04", pt.format("%"), "");
    a.checkEqual("05", pt.format("%E"), "");
    a.checkEqual("06", pt.format("%O"), "");

    // Single placeholders
    a.checkEqual("11", pt.format("%a"), "Sat");
    a.checkEqual("12", pt.format("%A"), "Saturday");
    a.checkEqual("13", pt.format("%b"), "Dec");
    a.checkEqual("14", pt.format("%B"), "December");
    a.checkEqual("15", pt.format("%c"), "2050-12-31 23:59:60");
    a.checkEqual("16", pt.format("%C"), "20");
    a.checkEqual("17", pt.format("%d"), "31");
    a.checkEqual("18", pt.format("%D"), "12/31/50");
    a.checkEqual("19", pt.format("%e"), "31");
    a.checkEqual("20", pt.format("%F"), "2050-12-31");
    a.checkEqual("21", pt.format("%h"), "Dec");
    a.checkEqual("22", pt.format("%H"), "23");
    a.checkEqual("23", pt.format("%I"), "11");
    a.checkEqual("24", pt.format("%m"), "12");
    a.checkEqual("25", pt.format("%M"), "59");
    a.checkEqual("26", pt.format("%n"), "\n");
    a.checkEqual("27", pt.format("%p"), "PM");
    a.checkEqual("28", pt.format("%r"), "11:59:60 PM");
    a.checkEqual("29", pt.format("%R"), "23:59");
    a.checkEqual("30", pt.format("%S"), "60");
    a.checkEqual("31", pt.format("%t"), "\t");
    a.checkEqual("32", pt.format("%T"), "23:59:60");
    a.checkEqual("33", pt.format("%u"), "6");
    a.checkEqual("34", pt.format("%w"), "6");
    a.checkEqual("35", pt.format("%x"), "2050-12-31");
    a.checkEqual("36", pt.format("%X"), "23:59:60");
    a.checkEqual("37", pt.format("%y"), "50");
    a.checkEqual("38", pt.format("%Y"), "2050");
    a.checkEqual("39", pt.format("%z"), "");
    a.checkEqual("40", pt.format("%Z"), "");
    a.checkEqual("41", pt.format("%%"), "%");
    a.checkEqual("42", pt.format("%_"), "_");
}

AFL_TEST("afl.sys.ParsedTime:format:2", a)
{
    afl::sys::ParsedTime pt;
    pt.m_year        = 1912;
    pt.m_month       = 3;
    pt.m_day         = 5;       // one-digit to test %d vs %e
    pt.m_hour        = 11;
    pt.m_minute      = 40;
    pt.m_second      = 20;
    pt.m_millisecond = 999;
    pt.m_weekday     = 0;

    // Simple cases
    a.checkEqual("01", pt.format(0), "");
    a.checkEqual("02", pt.format(""), "");
    a.checkEqual("03", pt.format("hi mom"), "hi mom");

    // Single placeholders
    a.checkEqual("11", pt.format("%a"), "Sun");
    a.checkEqual("12", pt.format("%A"), "Sunday");
    a.checkEqual("13", pt.format("%b"), "Mar");
    a.checkEqual("14", pt.format("%B"), "March");
    a.checkEqual("15", pt.format("%c"), "1912-03-05 11:40:20");
    a.checkEqual("16", pt.format("%C"), "19");
    a.checkEqual("17", pt.format("%d"), "05");
    a.checkEqual("18", pt.format("%D"), "03/05/12");
    a.checkEqual("19", pt.format("%e"), " 5");
    a.checkEqual("20", pt.format("%F"), "1912-03-05");
    a.checkEqual("21", pt.format("%h"), "Mar");
    a.checkEqual("22", pt.format("%H"), "11");
    a.checkEqual("23", pt.format("%I"), "11");
    a.checkEqual("24", pt.format("%m"), "03");
    a.checkEqual("25", pt.format("%M"), "40");
    a.checkEqual("26", pt.format("%p"), "AM");
    a.checkEqual("27", pt.format("%r"), "11:40:20 AM");
    a.checkEqual("28", pt.format("%R"), "11:40");
    a.checkEqual("29", pt.format("%S"), "20");
    a.checkEqual("30", pt.format("%T"), "11:40:20");
    a.checkEqual("31", pt.format("%u"), "7");
    a.checkEqual("32", pt.format("%w"), "0");
    a.checkEqual("33", pt.format("%x"), "1912-03-05");
    a.checkEqual("34", pt.format("%X"), "11:40:20");
    a.checkEqual("35", pt.format("%y"), "12");
    a.checkEqual("36", pt.format("%Y"), "1912");

    // Modifiers
    a.checkEqual("41", pt.format("%Ey"), "12");
    a.checkEqual("42", pt.format("%Oy"), "12");
    a.checkEqual("43", pt.format("%O%"), "%");
}

/** Test month formatting. */
AFL_TEST("afl.sys.ParsedTime:format:months", a)
{
    afl::sys::ParsedTime pt = afl::sys::ParsedTime();

    // In-range
    pt.m_month = 1;
    a.checkEqual("jan", pt.format("%b|%B"), "Jan|January");
    pt.m_month = 2;
    a.checkEqual("feb", pt.format("%b|%B"), "Feb|February");
    pt.m_month = 3;
    a.checkEqual("mar", pt.format("%b|%B"), "Mar|March");
    pt.m_month = 4;
    a.checkEqual("apr", pt.format("%b|%B"), "Apr|April");
    pt.m_month = 5;
    a.checkEqual("may", pt.format("%b|%B"), "May|May");
    pt.m_month = 6;
    a.checkEqual("jun", pt.format("%b|%B"), "Jun|June");
    pt.m_month = 7;
    a.checkEqual("jul", pt.format("%b|%B"), "Jul|July");
    pt.m_month = 8;
    a.checkEqual("aug", pt.format("%b|%B"), "Aug|August");
    pt.m_month = 9;
    a.checkEqual("sep", pt.format("%b|%B"), "Sep|September");
    pt.m_month = 10;
    a.checkEqual("oct", pt.format("%b|%B"), "Oct|October");
    pt.m_month = 11;
    a.checkEqual("nov", pt.format("%b|%B"), "Nov|November");
    pt.m_month = 12;
    a.checkEqual("dec", pt.format("%b|%B"), "Dec|December");

    // Out-of-range
    pt.m_month = 0;
    a.checkEqual("zero", pt.format("%b|%B"), "|");
    pt.m_month = 13;
    a.checkEqual("13", pt.format("%b|%B"), "|");
    pt.m_month = 1000;
    a.checkEqual("1000", pt.format("%b|%B"), "|");
}

/** Test weekday formatting. */
AFL_TEST("afl.sys.ParsedTime:format:weekdays", a)
{
    afl::sys::ParsedTime pt = afl::sys::ParsedTime();

    // In-range
    pt.m_weekday = 0;
    a.checkEqual("sun", pt.format("%a|%A|%u|%w"), "Sun|Sunday|7|0");
    pt.m_weekday = 1;
    a.checkEqual("mon", pt.format("%a|%A|%u|%w"), "Mon|Monday|1|1");
    pt.m_weekday = 2;
    a.checkEqual("tue", pt.format("%a|%A|%u|%w"), "Tue|Tuesday|2|2");
    pt.m_weekday = 3;
    a.checkEqual("wed", pt.format("%a|%A|%u|%w"), "Wed|Wednesday|3|3");
    pt.m_weekday = 4;
    a.checkEqual("thu", pt.format("%a|%A|%u|%w"), "Thu|Thursday|4|4");
    pt.m_weekday = 5;
    a.checkEqual("fri", pt.format("%a|%A|%u|%w"), "Fri|Friday|5|5");
    pt.m_weekday = 6;
    a.checkEqual("sat", pt.format("%a|%A|%u|%w"), "Sat|Saturday|6|6");

    // Out-of-range
    pt.m_weekday = 8;
    a.checkEqual("eight", pt.format("%a|%A|%u|%w"), "|||8");
}

/** Test hour formatting. */
AFL_TEST("afl.sys.ParsedTime:format:hours", a)
{
    afl::sys::ParsedTime pt = afl::sys::ParsedTime();

    pt.m_hour = 0;
    a.checkEqual("00", pt.format("%H|%I|%p"), "00|12|AM");
    pt.m_hour = 1;
    a.checkEqual("01", pt.format("%H|%I|%p"), "01|01|AM");
    pt.m_hour = 2;
    a.checkEqual("02", pt.format("%H|%I|%p"), "02|02|AM");
    pt.m_hour = 3;
    a.checkEqual("03", pt.format("%H|%I|%p"), "03|03|AM");
    pt.m_hour = 4;
    a.checkEqual("04", pt.format("%H|%I|%p"), "04|04|AM");
    pt.m_hour = 5;
    a.checkEqual("05", pt.format("%H|%I|%p"), "05|05|AM");
    pt.m_hour = 6;
    a.checkEqual("06", pt.format("%H|%I|%p"), "06|06|AM");
    pt.m_hour = 7;
    a.checkEqual("07", pt.format("%H|%I|%p"), "07|07|AM");
    pt.m_hour = 8;
    a.checkEqual("08", pt.format("%H|%I|%p"), "08|08|AM");
    pt.m_hour = 9;
    a.checkEqual("09", pt.format("%H|%I|%p"), "09|09|AM");
    pt.m_hour = 10;
    a.checkEqual("10", pt.format("%H|%I|%p"), "10|10|AM");
    pt.m_hour = 11;
    a.checkEqual("11", pt.format("%H|%I|%p"), "11|11|AM");
    pt.m_hour = 12;
    a.checkEqual("12", pt.format("%H|%I|%p"), "12|12|PM");
    pt.m_hour = 13;
    a.checkEqual("13", pt.format("%H|%I|%p"), "13|01|PM");
    pt.m_hour = 14;
    a.checkEqual("14", pt.format("%H|%I|%p"), "14|02|PM");
    pt.m_hour = 15;
    a.checkEqual("15", pt.format("%H|%I|%p"), "15|03|PM");
    pt.m_hour = 16;
    a.checkEqual("16", pt.format("%H|%I|%p"), "16|04|PM");
    pt.m_hour = 17;
    a.checkEqual("17", pt.format("%H|%I|%p"), "17|05|PM");
    pt.m_hour = 18;
    a.checkEqual("18", pt.format("%H|%I|%p"), "18|06|PM");
    pt.m_hour = 19;
    a.checkEqual("19", pt.format("%H|%I|%p"), "19|07|PM");
    pt.m_hour = 20;
    a.checkEqual("20", pt.format("%H|%I|%p"), "20|08|PM");
    pt.m_hour = 21;
    a.checkEqual("21", pt.format("%H|%I|%p"), "21|09|PM");
    pt.m_hour = 22;
    a.checkEqual("22", pt.format("%H|%I|%p"), "22|10|PM");
    pt.m_hour = 23;
    a.checkEqual("23", pt.format("%H|%I|%p"), "23|11|PM");
}
