/**
  *  \file test/afl/string/parsememorytest.cpp
  *  \brief Test for afl::string::ParseMemory
  */

#include "afl/string/parsememory.hpp"

#include "afl/string/char.hpp"
#include "afl/test/testrunner.hpp"

/** Simple tests. */
AFL_TEST("afl.string.ParseMemory", a)
{
    afl::string::ConstStringMemory_t t(afl::string::toMemory("foo =  123z"));
    a.checkEqual("01. size", t.size(), 11U);

    // Some literals
    a.checkEqual("11. parseLiteral", afl::string::parseLiteral(t, "bar"), 0U);
    a.checkEqual("12. size", t.size(), 11U);
    a.checkEqual("13. parseLiteral", afl::string::parseLiteral(t, "foa"), 0U);
    a.checkEqual("14. size", t.size(), 11U);
    a.checkEqual("15. parseLiteral", afl::string::parseLiteral(t, "foo"), 3U);
    a.checkEqual("16. size", t.size(), 8U);

    // Whitespace
    a.checkEqual("21. parseChar", afl::string::parseChar(t, 'x'), 0U);
    a.checkEqual("22. size", t.size(), 8U);
    a.checkEqual("23. parseChar", afl::string::parseChar(t, ' '), 1U);
    a.checkEqual("24. size", t.size(), 7U);

    // Equals
    a.checkEqual("31. parseChar", afl::string::parseChar(t, '='), 1U);
    a.checkEqual("32. size", t.size(), 6U);

    // Predicate
    a.checkEqual("41. parseIf", afl::string::parseIf(t, afl::string::charIsUpper), 0U);
    a.checkEqual("42. size", t.size(), 6U);
    a.checkEqual("43. parseIf", afl::string::parseIf(t, afl::string::charIsSpace), 2U);
    a.checkEqual("44. size", t.size(), 4U);

    // Number
    uint64_t val;
    a.checkEqual("51. parseNumber", afl::string::parseNumber(t, val), 3U);
    a.checkEqual("52. val", val, 123U);
    a.checkEqual("53. size. size", t.size(), 1U);

    // Remainder
    a.checkEqual("61. parseChar", afl::string::parseChar(t, 'x'), 0U);
    a.checkEqual("62. size", t.size(), 1U);
    a.checkEqual("63. parseChar", afl::string::parseChar(t, 'z'), 1U);
    a.checkEqual("64. size", t.size(), 0U);
}
