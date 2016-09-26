/**
  *  \file arch/time.hpp
  *  \brief System-dependant part of afl/sys/time.cpp
  *
  *  Provides a type TimeImpl_t with the following signatures:
  *
  *  - static int64_t packTime(const ParsedTime& parsedTime, Time::Kind kind)
  *    - convert ParsedTime to int64_t
  *  - static void unpackTime(afl::sys::ParsedTime& parsedTime, int64_t time, Time::Kind kind);
  *    - convert int64_t to ParsedTime
  *  - static String_t formatTime(int64_t time, Time::Kind kind, Time::Format fmt);
  *    - locale-specific formatting
  *  - static int64_t getCurrentTime();
  *    - get current wall-clock time as int64_t
  *  - static uint32_t getTickCounter();
  *    - get tick counter
  */
#ifndef AFL_ARCH_TIME_HPP
#define AFL_ARCH_TIME_HPP

#if TARGET_OS_POSIX
/*
 *  POSIX
 */
# include "arch/posix/posixtime.hpp"
namespace {
    typedef arch::posix::PosixTime TimeImpl_t;
}
#elif TARGET_OS_WIN32
/*
 *  Win32
 */
# include "arch/win32/win32time.hpp"
namespace {
    typedef arch::win32::Win32Time TimeImpl_t;
}
#else
# error Teach me about your time
#endif

#endif
