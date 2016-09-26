/**
  *  \file afl/sys/duration.hpp
  *  \brief Class afl::sys::Duration
  */
#ifndef AFL_AFL_SYS_DURATION_HPP
#define AFL_AFL_SYS_DURATION_HPP

#include "afl/base/types.hpp"
#include "afl/sys/types.hpp"

namespace afl { namespace sys {

    struct ParsedTime;

    /** Temporal duration.
        A duration is represented as a millisecond interval.
        We use 64 bits, allowing sufficiently large intervals for all practical purposes
        (32 bits would suffice for up to 49 days).

        A duration can be negative or zero as an intermediate result. */
    class Duration {
     public:
        /** Default constructor.
            Makes a zero duration. */
        Duration();

        /** Construct from ParsedTime.
            Builds a duration from the m_day, m_hour, m_minute, m_second, and m_millisecond fields of the ParsedTime.
            (The m_month and m_year fields cannot be sensibly used because months and years have different lengths;
            however, m_day is permitted to be larger than 31.)
            \param pt Time in components */
        explicit Duration(const ParsedTime& pt);

        /** Construct from milliseconds.
            \param time Number of milliseconds */
        explicit Duration(int64_t time);

        /** Compare for equality.
            \param other Other duration
            \return true iff both durations are equal */
        bool operator==(const Duration& other) const;

        /** Compare for inequality.
            \param other Other duration
            \return true iff both durations are different */
        bool operator!=(const Duration& other) const;

        /** Compare for shorter-or-equal.
            \param other Other duration
            \return true iff this duration is shorter or equal than the other */
        bool operator<=(const Duration& other) const;

        /** Compare for longer-or-equal.
            \param other Other duration
            \return true iff this duration is longer or equal than the other */
        bool operator>=(const Duration& other) const;

        /** Compare for shorter.
            \param other Other duration
            \return true iff this duration is strictly shorter than the other */
        bool operator<(const Duration& other) const;

        /** Compare for longer.
            \param other Other duration
            \return true iff this duration is strictly longer than the other */
        bool operator>(const Duration& other) const;

        /** Add durations.
            \param other Duration to add
            \return Sum duration */
        Duration operator+(const Duration& other) const;

        /** Add durations in-place.
            \param other Duration to add
            \return *this */
        Duration& operator+=(const Duration& other);

        /** Subtract durations.
            \param other Duration to subtract
            \return Difference duration */
        Duration operator-(const Duration& other) const;

        /** Subtract durations in-place.
            \param other Duration to subtract
            \return *this */
        Duration& operator-=(const Duration& other);

        /** Negate duration.
            \return negated duration */
        Duration operator-() const;

        /** Check validity.
            A duration is valid if it is not negative.
            \return true if duration is valid */
        bool isValid() const;

        /** Unpack duration.
            Splits this duration into m_millisecond, m_second, m_minute, m_hour, and m_day.
            The m_month, m_year, m_weekday fields are set to zero.
            Negative durations are reported as 0.
            \param pt [out] ParsedTime */
        void unpack(ParsedTime& pt) const;

        /** Get duration in milliseconds.
            \return Duration in milliseconds */
        int64_t getMilliseconds() const;

        /** Convert to Timeout_t.
            Negative durations are reported as 0, durations longer than 2**32-2 milliseconds
            (around 49 days) are reported as 2*32-2 milliseconds (largest finite timeout).
            \return timeout */
        Timeout_t toTimeout() const;

        /** Create from milliseconds.
            \param ms Millisecond count
            \return Duration representing that many milliseconds */
        static Duration fromMilliseconds(int64_t ms);

        /** Create from seconds.
            \param seconds Second count
            \return Duration representing that many seconds */
        static Duration fromSeconds(int64_t seconds);

        /** Create from minutes.
            \param minutes Minute count
            \return Duration representing that many minutes */
        static Duration fromMinutes(int64_t minutes);

        /** Create from hours.
            \param hours Hour count
            \return Duration representing that many hours */
        static Duration fromHours(int64_t hours);

     private:
        /** Duration in milliseconds. */
        int64_t m_duration;
    };

} }

/******************************** Inlines ********************************/

inline
afl::sys::Duration::Duration()
    : m_duration(0)
{ }

inline
afl::sys::Duration::Duration(int64_t time)
    : m_duration(time)
{ }

inline bool
afl::sys::Duration::operator==(const Duration& other) const
{
    return m_duration == other.m_duration;
}

inline bool
afl::sys::Duration::operator!=(const Duration& other) const
{
    return m_duration != other.m_duration;
}

inline bool
afl::sys::Duration::operator<=(const Duration& other) const
{
    return m_duration <= other.m_duration;
}

inline bool
afl::sys::Duration::operator>=(const Duration& other) const
{
    return m_duration >= other.m_duration;
}

inline bool
afl::sys::Duration::operator<(const Duration& other) const
{
    return m_duration < other.m_duration;
}

inline bool
afl::sys::Duration::operator>(const Duration& other) const
{
    return m_duration > other.m_duration;
}

inline afl::sys::Duration
afl::sys::Duration::operator+(const Duration& other) const
{
    return Duration(m_duration + other.m_duration);
}

inline afl::sys::Duration&
afl::sys::Duration::operator+=(const Duration& other)
{
    m_duration += other.m_duration;
    return *this;
}

inline afl::sys::Duration
afl::sys::Duration::operator-(const Duration& other) const
{
    return Duration(m_duration - other.m_duration);
}

inline afl::sys::Duration&
afl::sys::Duration::operator-=(const Duration& other)
{
    m_duration -= other.m_duration;
    return *this;
}

inline afl::sys::Duration
afl::sys::Duration::operator-() const
{
    return Duration(-m_duration);
}

inline bool
afl::sys::Duration::isValid() const
{
    return m_duration >= 0;
}

inline int64_t
afl::sys::Duration::getMilliseconds() const
{
    return m_duration;
}

inline afl::sys::Duration
afl::sys::Duration::fromMilliseconds(int64_t ms)
{
    return Duration(ms);
}

inline afl::sys::Duration
afl::sys::Duration::fromSeconds(int64_t seconds)
{
    return Duration(seconds * 1000);
}

inline afl::sys::Duration
afl::sys::Duration::fromMinutes(int64_t minutes)
{
    return Duration(minutes * 60 * 1000);
}

inline afl::sys::Duration
afl::sys::Duration::fromHours(int64_t hours)
{
    return Duration(hours * 60 * 60 * 1000);
}

#endif
