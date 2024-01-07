/**
  *  \file test/afl/string/hextest.cpp
  *  \brief Test for afl::string::Hex
  */

#include "afl/string/hex.hpp"

#include "afl/string/char.hpp"
#include "afl/test/testrunner.hpp"

/** Simple tests. */
AFL_TEST("afl.string.Hex", a)
{
    // Sanity check
    a.check("charIsUpper", afl::string::charIsUpper(afl::string::HEX_DIGITS_UPPER[10]));
    a.check("charIsLower", afl::string::charIsLower(afl::string::HEX_DIGITS_LOWER[10]));

    // Digit checks
    for (int i = 0; i < 16; ++i) {
        a.checkEqual("getHexDigitValue upper", afl::string::getHexDigitValue(afl::string::HEX_DIGITS_UPPER[i]), i);
        a.checkEqual("getHexDigitValue lower", afl::string::getHexDigitValue(afl::string::HEX_DIGITS_LOWER[i]), i);
    }

    // Format
    String_t s;
    afl::string::putHexByte(s, 1);
    a.checkEqual("putHexByte 1", s, "01");
    afl::string::putHexByte(s, 162);
    a.checkEqual("putHexByte 162", s, "01A2");
    afl::string::putHexByte(s, 243, afl::string::HEX_DIGITS_LOWER);
    a.checkEqual("putHexByte 243", s, "01A2f3");
}
