/**
  *  \file afl/sys/parsedtime.cpp
  *  \brief Structure afl::sys::ParsedTime
  */

#include "afl/sys/parsedtime.hpp"
#include "afl/string/format.hpp"
#include "afl/base/countof.hpp"

namespace {
    const char ABBREVIATED_WEEKDAY_NAMES[7][4] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
    const char FULL_WEEKDAY_NAMES[7][10]  = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
    const char WEEKDAY_DIGITS[7] = { '7', '1', '2', '3', '4', '5', '6' };

    const char ABBREVIATED_MONTH_NAMES[12][4] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    const char FULL_MONTH_NAMES[12][10] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };

    template<typename T, size_t N>
    inline void append(String_t& result, size_t n, T (&array)[N])
    {
        if (n < N) {
            result += array[n];
        }
    }

    void append(String_t& result, const char* fmt, uint16_t n)
    {
        result += afl::string::Format(fmt, n);
    }

    uint16_t toTwelve(uint16_t h)
    {
        if (h == 0) {
            return 12;
        } else if (h > 12) {
            return uint16_t(h - 12);
        } else {
            return h;
        }
    }
}


// Format into string (strftime).
String_t
afl::sys::ParsedTime::format(const char* fmt)
{
    // Prevent people shooting their feet
    if (fmt == 0) {
        return String_t();
    }

    // Produce result
    String_t result;
    while (const char ch = *fmt++) {
        if (ch == '%') {
            char key = *fmt++;
            if (key == 'E' || key == 'O') {
                key = *fmt++;
            }
            if (key == 0) {
                break;
            }
            switch (key) {
             case 'a': append(result, m_weekday, ABBREVIATED_WEEKDAY_NAMES); break; // abbreviated weekday name
             case 'A': append(result, m_weekday, FULL_WEEKDAY_NAMES);        break; // full weekday name
             case 'b': append(result, m_month-1, ABBREVIATED_MONTH_NAMES);   break; // abbbreviated month name
             case 'B': append(result, m_month-1, FULL_MONTH_NAMES);          break; // full month name
             case 'c': result += format("%Y-%m-%d %H:%M:%S");                break; // date and time
             case 'C': append(result, "%02d", m_year / 100);                 break; // year divided by 100 [00-99]
             case 'd': append(result, "%02d", m_day);                        break; // day-of-month [01-31]
             case 'D': result += format("%m/%d/%y");                         break; // shortcut
             case 'e': append(result, "%2d", m_day);                         break; // day-of-month [ 1-31]
             case 'F': result += format("%Y-%m-%d");                         break; // shortcut
             // case 'g': 2 digits of week-based year
             // case 'G': 4 digits of week-based year
             case 'h': append(result, m_month-1, ABBREVIATED_MONTH_NAMES);   break; // same as %b
             case 'H': append(result, "%02d", m_hour);                       break; // 24-hour hour [00-23]
             case 'I': append(result, "%02d", toTwelve(m_hour));             break; // 12-hour hour [01-12]
             // case 'j': 3 digits of day of year
             case 'm': append(result, "%02d", m_month);                      break; // month [01-12]
             case 'M': append(result, "%02d", m_minute);                     break; // minute [00-59]
             case 'n': result += '\n';                                       break; // newline
             case 'p': result += (m_hour < 12 ? "AM" : "PM");                break; // AM/PM
             case 'r': result += format("%I:%M:%S %p");                      break; // 12-hour time
             case 'R': result += format("%H:%M");                            break; // shortcut
             case 'S': append(result, "%02d", m_second);                     break; // second [00,60]
             case 't': result += '\t';                                       break; // tab
             case 'T': result += format("%H:%M:%S");                         break; // shortcut
             case 'u': append(result, m_weekday, WEEKDAY_DIGITS);            break; // weekday digits [1,7]
             // case 'U': 2 digits of week number based on Sundays
             // case 'V': 2 digits of week number
             case 'w': append(result, "%d", m_weekday);                      break; // weekday digits [0,6]
             // case 'W': 2 digits of week number based on Mondays
             case 'x': result += format("%Y-%m-%d");                         break; // date
             case 'X': result += format("%H:%M:%S");                         break; // time
             case 'y': append(result, "%02d", m_year % 100);                 break; // year [00-99]
             case 'Y': append(result, "%d", m_year);                         break; // full year
             case 'z':                                                       break; // UTC offset, nothing if not known
             case 'Z':                                                       break; // timezone name, nothing if not known
             case '%': result += key;                                        break; // percent
             default:  result += key;                                        break;
            }
        } else {
            result += ch;
        }
    }
    return result;
}
