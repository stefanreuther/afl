/**
  *  \file arch/win32/win32time.cpp
  *  \brief Class arch::win32::Win32Time
  */

#if TARGET_OS_WIN32
#include <windows.h>
#include "arch/win32/win32.hpp"
#include "arch/win32/win32time.hpp"
#include "afl/sys/parsedtime.hpp"

namespace {
    // Trim trailing nulls. GetDateFormat / GetTimeFormat produce these.
    int trim(const char* buffer, int n)
    {   
        while (n > 0 && buffer[n-1] == 0) {
            --n;
        }
        return n;
    }
}

// Convert ParsedTime to int64_t
int64_t
arch::win32::Win32Time::packTime(const afl::sys::ParsedTime& parsedTime, afl::sys::Time::Kind kind)
{
    // Convert ParsedTime to SYSTEMTIME
    SYSTEMTIME sysTime;
    sysTime.wYear         = parsedTime.m_year;
    sysTime.wMonth        = parsedTime.m_month;
    sysTime.wDayOfWeek    = 0;
    sysTime.wDay          = parsedTime.m_day;
    sysTime.wHour         = parsedTime.m_hour;
    sysTime.wMinute       = parsedTime.m_minute;
    sysTime.wSecond       = parsedTime.m_second;
    sysTime.wMilliseconds = parsedTime.m_millisecond;

    // Convert SYSTEMTIME to FILETIME
    FILETIME fileTime;
    if (!SystemTimeToFileTime(&sysTime, &fileTime)) {
        return 0;
    }

    // If local conversion is requested, convert from local time
    if (kind == afl::sys::Time::LocalTime) {
        FILETIME universalTime;
        if (!LocalFileTimeToFileTime(&fileTime, &universalTime)) {
            return 0;
        }
        return fromFileTime(universalTime);
    } else {
        return fromFileTime(fileTime);
    }
}

// Convert int64_t to Win32Time
void
arch::win32::Win32Time::unpackTime(afl::sys::ParsedTime& parsedTime, int64_t time, afl::sys::Time::Kind kind)
{
    SYSTEMTIME sysTime;
    if (!toSystemTime(sysTime, time, kind)) {
        // Error return
        parsedTime = afl::sys::ParsedTime();
    } else {
        // Regular return
        parsedTime.m_year        = sysTime.wYear;
        parsedTime.m_month       = sysTime.wMonth;
        parsedTime.m_day         = sysTime.wDay;
        parsedTime.m_hour        = sysTime.wHour;
        parsedTime.m_minute      = sysTime.wMinute;
        parsedTime.m_second      = sysTime.wSecond;
        parsedTime.m_weekday     = sysTime.wDayOfWeek;
        parsedTime.m_millisecond = sysTime.wMilliseconds;
    }
}

// Convert int64_t to human-readable string
String_t
arch::win32::Win32Time::formatTime(int64_t time, afl::sys::Time::Kind kind, afl::sys::Time::Format fmt)
{
    // Convert to SYSTEMTIME
    SYSTEMTIME sysTime;
    if (!toSystemTime(sysTime, time, kind)) {
        return String_t();
    }

    // Format it
    String_t result;
    char buffer[100];
    switch (fmt) {
     case afl::sys::Time::FullFormat:
        if (int n = GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &sysTime, 0, buffer, sizeof(buffer))) {
            result.append(buffer, trim(buffer, n));
            result += ", ";
        }
        if (int n = GetTimeFormat(LOCALE_USER_DEFAULT, 0, &sysTime, 0, buffer, sizeof(buffer))) {
            result.append(buffer, trim(buffer, n));
        }
        break;
     case afl::sys::Time::DateFormat:
        if (int n = GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &sysTime, 0, buffer, sizeof(buffer))) {
            result.append(buffer, trim(buffer, n));
        }
        break;
     case afl::sys::Time::TimeFormat:
        if (int n = GetTimeFormat(LOCALE_USER_DEFAULT, 0, &sysTime, 0, buffer, sizeof(buffer))) {
            result.append(buffer, trim(buffer, n));
        }
        break;
    }
    return convertFromANSI(afl::string::toMemory(result));
}

// Get current time
int64_t
arch::win32::Win32Time::getCurrentTime()
{
    FILETIME fileTime;
    GetSystemTimeAsFileTime(&fileTime);
    return fromFileTime(fileTime);
}

// Get tick counter
uint32_t
arch::win32::Win32Time::getTickCounter()
{
    return ::GetTickCount();
}

// Convert FILETIME to int64_t
int64_t
arch::win32::Win32Time::fromFileTime(const FILETIME& fileTime)
{
    // Compute ticks in 100-ns units
    uint64_t ticks = fileTime.dwLowDateTime + (uint64_t(fileTime.dwHighDateTime) << 32);

    // Convert to milliseconds
    return int64_t(ticks / 10000);
}

// Convert int64_t to FILETIME
void
arch::win32::Win32Time::toFileTime(FILETIME& fileTime, uint64_t time)
{
    uint64_t ticks = uint64_t(time * 10000);
    fileTime.dwLowDateTime = uint32_t(ticks);
    fileTime.dwHighDateTime = uint32_t(ticks >> 32);
}

// Convert int64_t to SYSTEMTIME
bool
arch::win32::Win32Time::toSystemTime(SYSTEMTIME& sysTime, uint64_t time, afl::sys::Time::Kind kind)
{
    // Convert from internal representation to FILETIME
    FILETIME fileTime;
    toFileTime(fileTime, time);

    // Convert to SYSTEMTIME
    if (kind == afl::sys::Time::LocalTime) {
        FILETIME localTime;
        return FileTimeToLocalFileTime(&fileTime, &localTime)
            && FileTimeToSystemTime(&localTime, &sysTime);
    } else {
        return FileTimeToSystemTime(&fileTime, &sysTime);
    }
}

#else
int g_variableToMakeWin32TimeObjectFileNotEmpty;
#endif
