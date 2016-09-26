/**
  *  \file u/t_string_parsememory.cpp
  *  \brief Test for afl/string/parsememory.hpp
  */

#include "afl/string/parsememory.hpp"

#include "u/t_string.hpp"
#include "afl/string/char.hpp"

/** Simple tests. */
void
TestStringParseMemory::testIt()
{
    afl::string::ConstStringMemory_t t(afl::string::toMemory("foo =  123z"));
    TS_ASSERT_EQUALS(t.size(), 11U);

    // Some literals
    TS_ASSERT_EQUALS(afl::string::parseLiteral(t, "bar"), 0U);
    TS_ASSERT_EQUALS(t.size(), 11U);
    TS_ASSERT_EQUALS(afl::string::parseLiteral(t, "foa"), 0U);
    TS_ASSERT_EQUALS(t.size(), 11U);
    TS_ASSERT_EQUALS(afl::string::parseLiteral(t, "foo"), 3U);
    TS_ASSERT_EQUALS(t.size(), 8U);

    // Whitespace
    TS_ASSERT_EQUALS(afl::string::parseChar(t, 'x'), 0U);
    TS_ASSERT_EQUALS(t.size(), 8U);
    TS_ASSERT_EQUALS(afl::string::parseChar(t, ' '), 1U);
    TS_ASSERT_EQUALS(t.size(), 7U);

    // Equals
    TS_ASSERT_EQUALS(afl::string::parseChar(t, '='), 1U);
    TS_ASSERT_EQUALS(t.size(), 6U);

    // Predicate
    TS_ASSERT_EQUALS(afl::string::parseIf(t, afl::string::charIsUpper), 0U);
    TS_ASSERT_EQUALS(t.size(), 6U);
    TS_ASSERT_EQUALS(afl::string::parseIf(t, afl::string::charIsSpace), 2U);
    TS_ASSERT_EQUALS(t.size(), 4U);

    // Number
    uint64_t val;
    TS_ASSERT_EQUALS(afl::string::parseNumber(t, val), 3U);
    TS_ASSERT_EQUALS(val, 123U);
    TS_ASSERT_EQUALS(t.size(), 1U);

    // Remainder
    TS_ASSERT_EQUALS(afl::string::parseChar(t, 'x'), 0U);
    TS_ASSERT_EQUALS(t.size(), 1U);
    TS_ASSERT_EQUALS(afl::string::parseChar(t, 'z'), 1U);
    TS_ASSERT_EQUALS(t.size(), 0U);
}
