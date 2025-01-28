/**
  *  \file afl/test/assert.cpp
  *  \brief Class afl::test::Assert
  */

#include <cmath>
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
afl::test::Assert::checkNull(const String_t& info, const void* p) const
{
    if (p != 0) {
        fail(afl::string::Format("%s (found %p != nullptr)", info, p));
    }
}

void
afl::test::Assert::checkNear(const String_t& info, double value, double expected, double range) const
{
    if (std::abs(value - expected) > range) {
        fail(afl::string::Format("%s (found %.2f differ from %.2f by more than %.2f)", info, value, expected, range));
    }
}

void
afl::test::Assert::checkContains(const String_t& info, const String_t& haystack, const String_t& needle) const
{
    if (haystack.find(needle) == String_t::npos) {
        fail(afl::string::Format("%s (found '%s', does not contain '%s')", info, haystack, needle));
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

const String_t&
afl::test::Assert::getLocation() const
{
    return m_location;
}
