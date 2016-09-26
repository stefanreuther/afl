/**
  *  \file afl/sys/duration.cpp
  *  \brief Class afl::sys::Duration
  */

#include "afl/sys/duration.hpp"
#include "afl/sys/parsedtime.hpp"
#include "afl/sys/types.hpp"

namespace {
    const int64_t MSECS_PER_DAY = 1000 * 60 * 60 * 24;
}

afl::sys::Duration::Duration(const ParsedTime& pt)
    // millisecond/second/minute/hour cannot overflow, because a day has just 86400000 milliseconds.
    // Only days need 64-bit precision.
    : m_duration(int32_t(pt.m_millisecond)
                 + 1000 * (int32_t(pt.m_second)
                           + 60 * (int32_t(pt.m_minute)
                                   + 60 * int32_t(pt.m_hour)))
                 + int64_t(pt.m_day) * MSECS_PER_DAY)
{ }

void
afl::sys::Duration::unpack(ParsedTime& pt) const
{
    int64_t val = (m_duration < 0 ? 0 : m_duration);
    pt.m_year    = 0;
    pt.m_month   = 0;
    pt.m_weekday = 0;
    pt.m_day     = static_cast<uint16_t>(val / MSECS_PER_DAY);

    int32_t rem = static_cast<int32_t>(val % MSECS_PER_DAY);
    pt.m_millisecond = static_cast<uint16_t>(rem % 1000);
    rem /= 1000;
    pt.m_second = static_cast<uint16_t>(rem % 60);
    rem /= 60;
    pt.m_minute = static_cast<uint16_t>(rem % 60);
    rem /= 60;
    pt.m_hour = static_cast<uint16_t>(rem);
}

afl::sys::Timeout_t
afl::sys::Duration::toTimeout() const
{
    if (m_duration < 0) {
        return 0;
    } else if (m_duration >= INFINITE_TIMEOUT) {
        return INFINITE_TIMEOUT-1;
    } else {
        return uint32_t(m_duration);
    }
}
