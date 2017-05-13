/**
  *  \file afl/sys/parsedtime.hpp
  *  \brief Structure afl::sys::ParsedTime
  */
#ifndef AFL_AFL_SYS_PARSEDTIME_HPP
#define AFL_AFL_SYS_PARSEDTIME_HPP

#include "afl/base/types.hpp"
#include "afl/string/string.hpp"

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

        /** Format into string (strftime).
            This function does not depend on the locale.
            It implements a subset of strftime.

            Placeholders start with a '%'.
            An 'E' or 'O' modifier is ignored.
            The '%' is then followed by a format letter.
            The following subset of strftime's format letters are implemented.
            As a rule of thumb, everything that does not need knowledge of a calendar is implemented.
            - 'a': abbreviated weekday name
            - 'A': full weekday name
            - 'b': abbbreviated month name
            - 'B': full month name
            - 'c': date and time (implemented as"%Y-%m-%d %H:%M:%S")
            - 'C': century [00,99]
            - 'd': day-of-month with leading zero [01-31]
            - 'D': defined as shortcut for "%m/%d/%y"
            - 'e': day-of-month with leading space [ 1-31]
            - 'F': defined as shortcut for "%Y-%m-%d"
            - 'h': defined as alias for "%b"
            - 'H': hour in 24-hour clock [00-23]
            - 'I': hour in 12-hour clock [01-12]
            - 'm': month [01-12]
            - 'M': minute [00-59]
            - 'n': newline
            - 'p': AM/PM indicator
            - 'r': time in 12-hour format (implemented as "%I:%M:%s %p")
            - 'R': defined as shortcut for "%H:%M"
            - 'S': second [00-60]
            - 't': tab
            - 'T': defined as shortcut for "%H:%M:%S"
            - 'u': weekday digits [1,7]
            - 'w': weekday digits [0,6]
            - 'x': date (implemented as "%Y-%m-%d")
            - 'X': time (implemented as "%H:%M:%S")
            - 'y': 2-digit year [00,99]
            - 'Y': full year
            - 'z': UTC offset (ignored)
            - 'Z': timezone name (ignored)
            - '%': percent sign

            Unimplemented:
            - 'g': 2 digits of week-based year
            - 'G': 4 digits of week-based year
            - 'j': 3 digits day-of-year
            - 'U': 2 digits of week number based on Sundays
            - 'V': 2 digits of week number
            - 'W': 2 digits of week number based on Mondays

            \param fmt Format string. */
        String_t format(const char* fmt);
    };

} }

#endif
