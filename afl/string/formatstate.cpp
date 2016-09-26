/**
  *  \file afl/string/formatstate.cpp
  *  \brief Class afl::string::FormatState
  */

#include "afl/string/formatstate.hpp"

afl::string::FormatState::FormatState()
    : m_code(),
      m_width(0),
      m_precision(0),
      m_flags(),
      m_conditions()
{ }

void
afl::string::FormatState::clear()
{
    m_code = '\0';
    m_width = 0;
    m_precision = 0;
    m_flags.clear();
}

char
afl::string::FormatState::getCode() const
{
    return m_code;
}

void
afl::string::FormatState::setCode(char code)
{
    m_code = code;
}

size_t
afl::string::FormatState::getWidth() const
{
    return m_width;
}

void
afl::string::FormatState::setWidth(size_t width)
{
    m_width = width;
}

size_t
afl::string::FormatState::getPrecision() const
{
    return m_precision;
}

void
afl::string::FormatState::setPrecision(size_t prec)
{
    m_precision = prec;
}

bool
afl::string::FormatState::hasFlag(Flag which) const
{
    return m_flags.contains(which);
}

void
afl::string::FormatState::setFlag(Flag which)
{
    m_flags += which;
}

void
afl::string::FormatState::clearFlags()
{
    m_flags.clear();
}

bool
afl::string::FormatState::hasCondition(Condition which) const
{
    return m_conditions.contains(which);
}

void
afl::string::FormatState::setCondition(Condition which, bool value)
{
    if (value) {
        m_conditions += which;
    } else {
        m_conditions -= which;
    }
}


void
afl::string::FormatState::setNumberConditions(ConstStringMemory_t data)
{
    // Clear old states
    setCondition(ZeroCondition, false);
    setCondition(OneCondition, false);

    // FIXME:
    const char* p = data.unsafeData();
    size_t n = data.size();

    // Find a digit that is not '0'
    while (n > 0 && *p != 0 && !(*p >= '1' && *p <= '9')) {
        ++p, --n;
    }

    // Skip optional following digits
    int value = 0;
    int digits = 0;
    while (n > 0 && *p != 0) {
        if (*p >= '0' && *p <= '9') {
            value = *p - '0';
            ++digits;
        }
        ++p, --n;
    }

    // Set new states
    if (digits == 0) {
        setCondition(ZeroCondition, true);
    } else if (digits == 1 && value == 1) {
        setCondition(OneCondition, true);
    } else {
        // no condition
    }
}
