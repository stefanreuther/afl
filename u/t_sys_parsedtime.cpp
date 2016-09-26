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
