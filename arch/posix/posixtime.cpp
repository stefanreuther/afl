/**
  *  \file arch/posix/posixtime.cpp
  *  \brief Class arch::posix::PosixTime
  */

#if TARGET_OS_POSIX
# define _POSIX_C_SOURCE 199506    /* get clock_gettime etc. */
#include <sys/time.h>
#include <time.h>
#include "arch/posix/posixtime.hpp"
#include "afl/sys/parsedtime.hpp"
#include "arch/posix/posix.hpp"

// Convert ParsedTime to int64_t
int64_t
arch::posix::PosixTime::packTime(const afl::sys::ParsedTime& parsedTime, afl::sys::Time::Kind kind)
{
    if (kind == afl::sys::Time::LocalTime) {
        // Local time: use mktime
        struct tm t;
        t.tm_sec   = parsedTime.m_second;
        t.tm_min   = parsedTime.m_minute;
        t.tm_hour  = parsedTime.m_hour;
        t.tm_mday  = parsedTime.m_day;
        t.tm_mon   = parsedTime.m_month - 1;
        t.tm_year  = parsedTime.m_year - 1900;
        t.tm_wday  = 0;
        t.tm_yday  = 0;
        t.tm_isdst = -1;

        return fromSysTime(::mktime(&t), parsedTime.m_millisecond);
    } else {
        // Manual conversion for UTC

        // Fetch and adjust parameters
        int32_t year  = parsedTime.m_year;
        int32_t month = parsedTime.m_month-1;
        int32_t day   = parsedTime.m_day-1;

        year += month / 12;
        month %= 12;
        
        static const uint16_t daysSinceNewYear[12] = {
            // Ja Feb Mar  Apr  May June July  Aug  Sep  Oct  Nov
            // 31 +28 +31  +30  +31  +30  +31  +31  +30  +31  +30
            0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
        };

        // Number of leap years since 1970
        int32_t leapYearsSinceEpoch
            = (year - 1967) / 4
            - (year - 1899) / 100
            + (year - 1599) / 400;

        // Days since 1970
        int32_t daysSinceEpoch = (year - 1970)*365 + leapYearsSinceEpoch + daysSinceNewYear[month] + day;
        if (month >= 2 && year%4 == 0 && (year%100 != 0 || year%400 == 0)) {
            ++daysSinceEpoch;
        }

        // Result
        return parsedTime.m_millisecond + 1000*(parsedTime.m_second + 60*(parsedTime.m_minute + 60*(parsedTime.m_hour + 24LL*daysSinceEpoch)));
    }
}

// Convert int64_t to PosixTime
void
arch::posix::PosixTime::unpackTime(afl::sys::ParsedTime& parsedTime, int64_t time, afl::sys::Time::Kind kind)
{
    // Convert to time_t (also keep milliseconds)
    time_t sysTime = toSysTime(time, parsedTime.m_millisecond);

    // Break into struct tm
    struct tm t;
    struct tm* pt = (kind == afl::sys::Time::LocalTime
                     ? ::localtime_r(&sysTime, &t)
                     : ::gmtime_r(&sysTime, &t));
    if (pt == 0) {
        // Error return
        parsedTime = afl::sys::ParsedTime();
    } else {
        // Regular return
        parsedTime.m_year    = static_cast<uint16_t>(1900 + t.tm_year);
        parsedTime.m_month   = static_cast<uint16_t>(1 + t.tm_mon);
        parsedTime.m_day     = static_cast<uint16_t>(t.tm_mday);
        parsedTime.m_hour    = static_cast<uint16_t>(t.tm_hour);
        parsedTime.m_minute  = static_cast<uint16_t>(t.tm_min);
        parsedTime.m_second  = static_cast<uint16_t>(t.tm_sec);
        parsedTime.m_weekday = static_cast<uint16_t>(t.tm_wday);
    }
}

// Convert int64_t to human-readable string
String_t
arch::posix::PosixTime::formatTime(int64_t time, afl::sys::Time::Kind kind, afl::sys::Time::Format fmt)
{
    // Convert to time_t (also keep milliseconds)
    uint16_t ms;
    time_t sysTime = toSysTime(time, ms);

    // Break into struct tm
    struct tm t;
    struct tm* pt = (kind == afl::sys::Time::LocalTime
                     ? ::localtime_r(&sysTime, &t)
                     : ::gmtime_r(&sysTime, &t));

    // Figure out format string
    const char* fmtStr = 0;
    switch (fmt) {
     case afl::sys::Time::FullFormat: fmtStr = "%c"; break;
     case afl::sys::Time::DateFormat: fmtStr = "%x"; break;
     case afl::sys::Time::TimeFormat: fmtStr = "%X"; break;
    }

    // Do it!
    if (fmtStr == 0 || pt == 0) {
        // Error
        return String_t();
    } else {
        // Valid
        char tmp[100];
        size_t n = ::strftime(tmp, sizeof(tmp), fmtStr, pt);
        return convertExternalToUtf8(afl::string::ConstStringMemory_t::unsafeCreate(tmp, n));
    }
}

// Get current time
int64_t
arch::posix::PosixTime::getCurrentTime()
{
    struct timeval tv;
    if (::gettimeofday(&tv, 0) == 0) {
        return fromSysTime(tv.tv_sec, static_cast<uint16_t>(tv.tv_usec / 1000));
    } else {
        return 0;
    }
}

// Get tick counter
uint32_t
arch::posix::PosixTime::getTickCounter()
{
#ifdef CLOCK_MONOTONIC
    // Try CLOCK_MONOTONIC, which is most reliable
    struct timespec ts;
    if (::clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
        return uint32_t(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
    }
#endif
    // We don't have CLOCK_MONOTONIC, or it failed; use current time
    return uint32_t(getCurrentTime());
}

// Convert system's time representation to int64_t
int64_t
arch::posix::PosixTime::fromSysTime(time_t t, uint16_t msecs)
{
    // Convert seconds to milliseconds, keeping the epoch
    return int64_t(t) * 1000 + msecs;
}

// Convert int64_t to system's time representation
time_t
arch::posix::PosixTime::toSysTime(int64_t t, uint16_t& msecsOut)
{
    msecsOut = uint16_t(t % 1000);
    return time_t(t / 1000);
}

#else
int g_variableToMakePosixTimeObjectFileNotEmpty;
#endif
