/**
  *  \file afl/sys/atomicinteger.cpp
  *  \brief Class afl::sys::AtomicInteger
  */

#include "afl/sys/atomicinteger.hpp"
#include "arch/atomicinteger.hpp"

afl::sys::AtomicInteger&
afl::sys::AtomicInteger::operator=(uint32_t val)
{
    setAtomic(m_val, val);
    return *this;
}

afl::sys::AtomicInteger::operator uint32_t() const
{
    return getAtomic(m_val);
}

uint32_t
afl::sys::AtomicInteger::operator+=(uint32_t delta)
{
    return incAtomic(m_val, delta);
}

bool
afl::sys::AtomicInteger::replaceIfEqual(uint32_t compare, uint32_t newVal)
{
    return casAtomic(m_val, compare, newVal);
}
