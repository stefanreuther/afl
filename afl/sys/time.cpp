/**
  *  \file afl/sys/time.cpp
  *  \brief Class afl::sys::Time
  */

#include "afl/sys/time.hpp"
#include "arch/time.hpp"
#include "afl/sys/parsedtime.hpp"

namespace {
    const afl::sys::ParsedTime unixZero = { 1970, 1, 1, 0, 0, 0, 0, 0 };
}

afl::sys::Time::Time(const ParsedTime& time, Kind kind)
    : m_time(TimeImpl_t::packTime(time, kind))
{ }

void
afl::sys::Time::unpack(ParsedTime& time, Kind kind) const
{
    TimeImpl_t::unpackTime(time, m_time, kind);
}

afl::sys::Time
afl::sys::Time::getCurrentTime()
{
    return Time(TimeImpl_t::getCurrentTime());
}

uint32_t
afl::sys::Time::getTickCounter()
{
    return TimeImpl_t::getTickCounter();
}

int64_t
afl::sys::Time::getUnixTime() const
{
    return (m_time - TimeImpl_t::packTime(unixZero, UniversalTime)) / 1000;
}

String_t
afl::sys::Time::toString(Kind kind, Format fmt) const
{
    return TimeImpl_t::formatTime(m_time, kind, fmt);
}

afl::sys::Time
afl::sys::Time::fromUnixTime(int64_t unixTime)
{
    return Time(TimeImpl_t::packTime(unixZero, UniversalTime) + 1000*unixTime);
}
