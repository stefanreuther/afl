/**
  *  \file arch/win32/win32time.hpp
  *  \brief Class arch::win32::Win32Time
  */
#ifndef AFL_ARCH_WIN32_WIN32TIME_HPP
#define AFL_ARCH_WIN32_WIN32TIME_HPP

#include <windows.h>
#include "afl/base/types.hpp"
#include "afl/sys/time.hpp"
#include "afl/string/string.hpp"

namespace arch { namespace win32 {

    /** Implementation of TimeImpl_t for Win32. */
    class Win32Time {
     public:
        // Official interface
        static int64_t packTime(const afl::sys::ParsedTime& parsedTime, afl::sys::Time::Kind kind);
        static void unpackTime(afl::sys::ParsedTime& parsedTime, int64_t time, afl::sys::Time::Kind kind);
        static String_t formatTime(int64_t time, afl::sys::Time::Kind kind, afl::sys::Time::Format fmt);
        static int64_t getCurrentTime();
        static uint32_t getTickCounter();

        // Internal interface (for use by classes in arch::win32 only)
        static int64_t fromFileTime(const FILETIME& fileTime);
        static void toFileTime(FILETIME& fileTime, uint64_t time);
        static bool toSystemTime(SYSTEMTIME& sysTime, uint64_t time, afl::sys::Time::Kind kind);
    };

} }

#endif
