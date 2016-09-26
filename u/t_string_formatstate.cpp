/**
  *  \file u/t_string_formatstate.cpp
  *  \brief Test for afl::string::FormatState
  */

#include "afl/string/formatstate.hpp"

#include "u/t_string.hpp"
#include "afl/string/string.hpp"

/** Test accessors. */
void
TestStringFormatState::testAccessors()
{
    afl::string::FormatState fs;

    // Values
    fs.setCode('c');
    TS_ASSERT_EQUALS(fs.getCode(), 'c');
    fs.setCode('x');
    TS_ASSERT_EQUALS(fs.getCode(), 'x');

    fs.setWidth(0);
    TS_ASSERT_EQUALS(fs.getWidth(), 0U);
    fs.setWidth(999);
    TS_ASSERT_EQUALS(fs.getWidth(), 999U);

    fs.setPrecision(0);
    TS_ASSERT_EQUALS(fs.getPrecision(), 0U);
    fs.setPrecision(7777);
    TS_ASSERT_EQUALS(fs.getPrecision(), 7777U);

    // Flags
    fs.clearFlags();
    TS_ASSERT(!fs.hasFlag(fs.SignFlag));
    TS_ASSERT(!fs.hasFlag(fs.GroupFlag));

    fs.setFlag(fs.SignFlag);
    TS_ASSERT(fs.hasFlag(fs.SignFlag));
    TS_ASSERT(!fs.hasFlag(fs.GroupFlag));

    // Conditions
    fs.setCondition(fs.ZeroCondition, true);
    TS_ASSERT(fs.hasCondition(fs.ZeroCondition));
    fs.setCondition(fs.ZeroCondition, false);
    TS_ASSERT(!fs.hasCondition(fs.ZeroCondition));
}

/** Test setNumberConditions. */
void
TestStringFormatState::testNumberConditions()
{
    afl::string::FormatState fs;

    // Empty string is zero!
    fs.setNumberConditions(afl::string::toMemory(""));
    TS_ASSERT(fs.hasCondition(fs.ZeroCondition));
    TS_ASSERT(!fs.hasCondition(fs.OneCondition));

    // Trivial case
    fs.setNumberConditions(afl::string::toMemory("0"));
    TS_ASSERT(fs.hasCondition(fs.ZeroCondition));
    TS_ASSERT(!fs.hasCondition(fs.OneCondition));

    // Currency (sort-of)
    fs.setNumberConditions(afl::string::toMemory("$0.00"));
    TS_ASSERT(fs.hasCondition(fs.ZeroCondition));
    TS_ASSERT(!fs.hasCondition(fs.OneCondition));

    fs.setNumberConditions(afl::string::toMemory("$0.01"));
    TS_ASSERT(!fs.hasCondition(fs.ZeroCondition));
    TS_ASSERT(fs.hasCondition(fs.OneCondition));

    fs.setNumberConditions(afl::string::toMemory("$0.02"));
    TS_ASSERT(!fs.hasCondition(fs.ZeroCondition));
    TS_ASSERT(!fs.hasCondition(fs.OneCondition));

    // Misc.
    fs.setNumberConditions(afl::string::toMemory("1c"));
    TS_ASSERT(!fs.hasCondition(fs.ZeroCondition));
    TS_ASSERT(fs.hasCondition(fs.OneCondition));

    fs.setNumberConditions(afl::string::toMemory("2c"));
    TS_ASSERT(!fs.hasCondition(fs.ZeroCondition));
    TS_ASSERT(!fs.hasCondition(fs.OneCondition));

    fs.setNumberConditions(afl::string::toMemory("0001c"));
    TS_ASSERT(!fs.hasCondition(fs.ZeroCondition));
    TS_ASSERT(fs.hasCondition(fs.OneCondition));

    fs.setNumberConditions(afl::string::toMemory("0100c"));
    TS_ASSERT(!fs.hasCondition(fs.ZeroCondition));
    TS_ASSERT(!fs.hasCondition(fs.OneCondition));

    fs.setNumberConditions(afl::string::toMemory("1,000c"));
    TS_ASSERT(!fs.hasCondition(fs.ZeroCondition));
    TS_ASSERT(!fs.hasCondition(fs.OneCondition));
}
