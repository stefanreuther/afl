/**
  *  \file app/date.cpp
  *  \brief Sample Application: Show Current Time
  */

#include <cstdio>
#include "afl/sys/time.hpp"
#include "afl/sys/parsedtime.hpp"

static void
printTime(const char* ttl, afl::sys::Time t, afl::sys::Time::Kind k)
{
    // Manually format it:
    afl::sys::ParsedTime pt;
    t.unpack(pt, k);
    printf("%s: %d/%02d/%02d, %d:%02d:%02d.%03d\n",
           ttl,
           pt.m_year, pt.m_month, pt.m_day,
           pt.m_hour, pt.m_minute, pt.m_second, pt.m_millisecond);

    // Using system format:
    printf("  Formatted: %s\n", t.toString(k, t.FullFormat).c_str());
}

int main()
{
    afl::sys::Time t = afl::sys::Time::getCurrentTime();
    printTime("Local time", t, t.LocalTime);
    printTime("Universal time", t, t.UniversalTime);
    printf("Unix time: %lld\n", (long long) t.getUnixTime());
    printf("Representation: %lld\n", (long long) t.getRepresentation());
    printf("Tick counter: %d\n", (int) afl::sys::Time::getTickCounter());
    return 0;
}
