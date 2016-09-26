/**
  *  \file arch/posix/posixtime.hpp
  *  \brief Class arch::posix::PosixTime
  */
#ifndef AFL_ARCH_POSIX_POSIXTIME_HPP
#define AFL_ARCH_POSIX_POSIXTIME_HPP

#include <time.h>
#include "afl/base/types.hpp"
#include "afl/sys/time.hpp"
#include "afl/string/string.hpp"

namespace arch { namespace posix {

    /** Implementation of TimeImpl_t for POSIX. */
    class PosixTime {
     public:
        // Official interface
        static int64_t packTime(const afl::sys::ParsedTime& parsedTime, afl::sys::Time::Kind kind);
        static void unpackTime(afl::sys::ParsedTime& parsedTime, int64_t time, afl::sys::Time::Kind kind);
        static String_t formatTime(int64_t time, afl::sys::Time::Kind kind, afl::sys::Time::Format fmt);
        static int64_t getCurrentTime();
        static uint32_t getTickCounter();

        // Internal interface
        static int64_t fromSysTime(time_t t, uint16_t msecs);
        static time_t toSysTime(int64_t t, uint16_t& msecsOut);
    };

} }

#endif
