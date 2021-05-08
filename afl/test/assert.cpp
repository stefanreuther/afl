/**
  *  \file afl/test/assert.cpp
  *  \brief Class afl::test::Assert
  */

#include "afl/test/assert.hpp"
#include "afl/except/assertionfailedexception.hpp"

afl::test::Assert::Assert(const String_t& location)
    : m_location(location)
{ }

afl::test::Assert::Assert(const char* location)
    : m_location(location)
{ }

void
afl::test::Assert::check(const String_t& info, bool condition) const
{
    if (!condition) {
        fail(info + " (condition failed)");
    }
}

void
afl::test::Assert::fail(const String_t& info) const
{
    throw afl::except::AssertionFailedException(info, m_location);
}

afl::test::Assert
afl::test::Assert::operator()(const char* location) const
{
    return operator()(String_t(location));
}

afl::test::Assert
afl::test::Assert::operator()(const String_t& location) const
{
    return Assert(m_location + ": " + location);
}
