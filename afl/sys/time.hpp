/**
  *  \file afl/sys/time.hpp
  *  \brief Class afl::sys::Time
  */
#ifndef AFL_AFL_SYS_TIME_HPP
#define AFL_AFL_SYS_TIME_HPP

#include "afl/base/types.hpp"
#include "afl/sys/duration.hpp"
#include "afl/string/string.hpp"

namespace afl { namespace sys {

    struct ParsedTime;

    /** Time.
        Represents an absolute time as duration since a system-specific epoch, specified in milliseconds.

        Methods include obtaining the current system time,
        and conversion to and from a human-friendly parsed representation. */
    class Time {
     public:
        /** Kind of time conversion. */
        enum Kind {
            LocalTime,               /**< Local time as configured by user. */
            UniversalTime            /**< Universal time. */
        };

        /** Kind of time format. */
        enum Format {
            FullFormat,              /**< Full time, e.g.\ "Sun 19 Oct 2014 12:55:51 PM CEST" */
            DateFormat,              /**< Just the date, e.g.\ "10/19/2014" */
            TimeFormat               /**< Just the time, e.g.\ "12:55:51 PM" */
        };

        /** Default constructor.
            Makes an invalid time (representing the epoch). */
        Time();

        /** Construct from parsed time.
            \param time Parsed time.
            \param kind Interpretation of parsed time. */
        Time(const ParsedTime& time, Kind kind);

        /** Construct from millisecond value.
            \see getRepresentation
            \param time Time in milliseconds */
        explicit Time(int64_t time);

        /** Check validity.
            A Time object is considered valid if it contains a nonzero value,
            i.e. was constructed using a constructor other than the default constructor. */
        bool isValid() const;

        /** Compare for equality.
            \param other Other time
            \return true iff both times are equal */
        bool operator==(const Time& other) const;

        /** Compare for inequality.
            \param other Other time
            \return true iff both times are different */
        bool operator!=(const Time& other) const;

        /** Compare earlier.
            \param other Other time
            \return true iff this time is before than the other */
        bool operator<(const Time& other) const;

        /** Compare earlier or equal.
            \param other Other time
            \return true iff this time is before or equal to the other */
        bool operator<=(const Time& other) const;

        /** Compare later.
            \param other Other time
            \return true iff this time is after than the other */
        bool operator>(const Time& other) const;

        /** Compare later or equal.
            \param other Other time
            \return true iff this time is after or equal to the other */
        bool operator>=(const Time& other) const;

        /** Compute difference.
            \param other Time to subtract
            \return Duration between the two times */
        Duration operator-(const Time& other) const;

        /** Rewind time.
            \param other Duration to subtract
            \return New time */
        Time operator-(const Duration& other) const;

        /** Rewind time in-place.
            \param other Duration to subtract
            \return *this */
        Time& operator-=(const Duration& other);

        /** Advance time.
            \param other Duration to add
            \return New time */
        Time operator+(const Duration& other) const;

        /** Advance time in-place.
            \param other Duration to add
            \return *this */
        Time& operator+=(const Duration& other);

        /** Unpack time.
            Converts this time into a human-readable form.
            \param time [out] Unpacked time
            \param kind [in] Kind of conversion (local/universal time) */
        void unpack(ParsedTime& time, Kind kind) const;

        /** Get representation.
            Returns the time since epoch in milliseconds, a monotonically increasing value.
            \return time representation */
        int64_t getRepresentation() const;

        /** Get as Unix time.
            Returns the number of whole seconds since 1/Jan/1970.
            \return Unix time */
        int64_t getUnixTime() const;

        /** Format as string.
            \param kind Kind of time to format (local/universal)
            \param fmt How to format the time
            \return formatted time */
        String_t toString(Kind kind, Format fmt) const;

        /** Get time from Unix time.
            \return Converted time */
        static Time fromUnixTime(int64_t unixTime);

        /** Get current time.
            \return Time object representing current wall-clock time */
        static Time getCurrentTime();

        /** Get tick counter.
            Returns a free-running 32-bit millisecond counter.
            This value can be used to measure small time intervals such as double-click delay.

            Unlike getCurrentTime(), the tick counter is (intended to be) unaffected by user settings.
            It has no direct mapping to wall-clock time, no fixed epoch,
            and cannot be compared between different executions of a program.
            It is also expected to be a little more efficient than getCurrentTime().
            It may or may not actually have millisecond precision.

            \return tick counter */
        static uint32_t getTickCounter();

     private:
        /** Time in milliseconds.
            For absolute times, time since a platform-dependant epoch. */
        int64_t m_time;
    };

    /** Make Time printable for assertions.
        Returns the platform-specific representation.
        @return integer */
    inline int64_t makePrintable(const Time& t)
    {
        return t.getRepresentation();
    }

} }

inline
afl::sys::Time::Time()
    : m_time(0)
{ }

inline
afl::sys::Time::Time(int64_t time)
    : m_time(time)
{ }

inline bool
afl::sys::Time::isValid() const
{
    return m_time != 0;
}

inline bool
afl::sys::Time::operator==(const Time& other) const
{
    return m_time == other.m_time;
}

inline bool
afl::sys::Time::operator!=(const Time& other) const
{
    return m_time != other.m_time;
}

inline bool
afl::sys::Time::operator<(const Time& other) const
{
    return m_time < other.m_time;
}

inline bool
afl::sys::Time::operator<=(const Time& other) const
{
    return m_time <= other.m_time;
}

inline bool
afl::sys::Time::operator>(const Time& other) const
{
    return m_time > other.m_time;
}

inline bool
afl::sys::Time::operator>=(const Time& other) const
{
    return m_time >= other.m_time;
}

inline afl::sys::Duration
afl::sys::Time::operator-(const Time& other) const
{
    return Duration(m_time - other.m_time);
}

inline afl::sys::Time
afl::sys::Time::operator-(const Duration& other) const
{
    return Time(m_time - other.getMilliseconds());
}

inline afl::sys::Time&
afl::sys::Time::operator-=(const Duration& other)
{
    m_time -= other.getMilliseconds();
    return *this;
}

inline afl::sys::Time
afl::sys::Time::operator+(const Duration& other) const
{
    return Time(m_time + other.getMilliseconds());
}

inline afl::sys::Time&
afl::sys::Time::operator+=(const Duration& other)
{
    m_time += other.getMilliseconds();
    return *this;
}

inline int64_t
afl::sys::Time::getRepresentation() const
{
    return m_time;
}

#endif
