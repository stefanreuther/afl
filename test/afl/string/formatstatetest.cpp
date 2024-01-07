/**
  *  \file test/afl/string/formatstatetest.cpp
  *  \brief Test for afl::string::FormatState
  */

#include "afl/string/formatstate.hpp"

#include "afl/string/string.hpp"
#include "afl/test/testrunner.hpp"

/** Test accessors. */
AFL_TEST("afl.string.FormatState:accessors", a)
{
    afl::string::FormatState fs;

    // Values
    fs.setCode('c');
    a.checkEqual("01. getCode", fs.getCode(), 'c');
    fs.setCode('x');
    a.checkEqual("02. getCode", fs.getCode(), 'x');

    fs.setWidth(0);
    a.checkEqual("11. getWidth", fs.getWidth(), 0U);
    fs.setWidth(999);
    a.checkEqual("12. getWidth", fs.getWidth(), 999U);

    fs.setPrecision(0);
    a.checkEqual("21. getPrecision", fs.getPrecision(), 0U);
    fs.setPrecision(7777);
    a.checkEqual("22. getPrecision", fs.getPrecision(), 7777U);

    // Flags
    fs.clearFlags();
    a.check("31. hasFlag", !fs.hasFlag(fs.SignFlag));
    a.check("32. hasFlag", !fs.hasFlag(fs.GroupFlag));

    fs.setFlag(fs.SignFlag);
    a.check("33. hasFlag", fs.hasFlag(fs.SignFlag));
    a.check("34. hasFlag", !fs.hasFlag(fs.GroupFlag));

    // Conditions
    fs.setCondition(fs.ZeroCondition, true);
    a.check("41. hasCondition", fs.hasCondition(fs.ZeroCondition));
    fs.setCondition(fs.ZeroCondition, false);
    a.check("42. hasCondition", !fs.hasCondition(fs.ZeroCondition));
}

/** Test setNumberConditions. */
AFL_TEST("afl.string.FormatState:setNumberConditions", a)
{
    afl::string::FormatState fs;

    // Empty string is zero!
    fs.setNumberConditions(afl::string::toMemory(""));
    a.check("01. zero", fs.hasCondition(fs.ZeroCondition));
    a.check("02. one", !fs.hasCondition(fs.OneCondition));

    // Trivial case
    fs.setNumberConditions(afl::string::toMemory("0"));
    a.check("11. zero", fs.hasCondition(fs.ZeroCondition));
    a.check("12. one", !fs.hasCondition(fs.OneCondition));

    // Currency (sort-of)
    fs.setNumberConditions(afl::string::toMemory("$0.00"));
    a.check("21. zero", fs.hasCondition(fs.ZeroCondition));
    a.check("22. one", !fs.hasCondition(fs.OneCondition));

    fs.setNumberConditions(afl::string::toMemory("$0.01"));
    a.check("31. zero", !fs.hasCondition(fs.ZeroCondition));
    a.check("32. one", fs.hasCondition(fs.OneCondition));

    fs.setNumberConditions(afl::string::toMemory("$0.02"));
    a.check("41. zero", !fs.hasCondition(fs.ZeroCondition));
    a.check("42. one", !fs.hasCondition(fs.OneCondition));

    // Misc.
    fs.setNumberConditions(afl::string::toMemory("1c"));
    a.check("51. zero", !fs.hasCondition(fs.ZeroCondition));
    a.check("52. one", fs.hasCondition(fs.OneCondition));

    fs.setNumberConditions(afl::string::toMemory("2c"));
    a.check("61. zero", !fs.hasCondition(fs.ZeroCondition));
    a.check("62. one", !fs.hasCondition(fs.OneCondition));

    fs.setNumberConditions(afl::string::toMemory("0001c"));
    a.check("71. zero", !fs.hasCondition(fs.ZeroCondition));
    a.check("72. one", fs.hasCondition(fs.OneCondition));

    fs.setNumberConditions(afl::string::toMemory("0100c"));
    a.check("81. zero", !fs.hasCondition(fs.ZeroCondition));
    a.check("82. one", !fs.hasCondition(fs.OneCondition));

    fs.setNumberConditions(afl::string::toMemory("1,000c"));
    a.check("91. zero", !fs.hasCondition(fs.ZeroCondition));
    a.check("92. one", !fs.hasCondition(fs.OneCondition));
}
