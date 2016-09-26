/**
  *  \file afl/sys/parsedtime.hpp
  *  \brief Structure afl::sys::ParsedTime
  */
#ifndef AFL_AFL_SYS_PARSEDTIME_HPP
#define AFL_AFL_SYS_PARSEDTIME_HPP

#include "afl/base/types.hpp"

namespace afl { namespace sys {

    /** Parsed time.
        Represents a time broken down into components.
        Classes Time and Duration work with these structures. */
    struct ParsedTime {
        /** Year. Absolute number, i.e. "1989" or "2015" (zero for Duration). */
        uint16_t m_year;

        /** Month. Number from 1..12 (zero for Duration). */
        uint16_t m_month;

        /** Day. Number from 1..31 (larger for Duration). */
        uint16_t m_day;

        /** Hour. Number from 0..23. */
        uint16_t m_hour;

        /** Minute. Number from 0..59. */
        uint16_t m_minute;

        /** Second. Number from 0 to 61. */
        uint16_t m_second;

        /** Millisecond. Number from 0 to 999. */
        uint16_t m_millisecond;

        /** Weekday. 0=Sunday to 6=Saturday. */
        uint16_t m_weekday;
    };

} }

#endif
