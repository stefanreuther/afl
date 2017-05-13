/**
  *  \file u/t_string_hex.cpp
  *  \brief Test for afl::string::Hex
  */

#include "afl/string/hex.hpp"

#include "t_string.hpp"
#include "afl/string/char.hpp"

/** Simple tests. */
void
TestStringHex::testIt()
{
    // Sanity check
    TS_ASSERT(afl::string::charIsUpper(afl::string::HEX_DIGITS_UPPER[10]));
    TS_ASSERT(afl::string::charIsLower(afl::string::HEX_DIGITS_LOWER[10]));

    // Digit checks
    for (int i = 0; i < 16; ++i) {
        TS_ASSERT_EQUALS(afl::string::getHexDigitValue(afl::string::HEX_DIGITS_UPPER[i]), i);
        TS_ASSERT_EQUALS(afl::string::getHexDigitValue(afl::string::HEX_DIGITS_LOWER[i]), i);
    }

    // Format
    String_t s;
    afl::string::putHexByte(s, 1);
    TS_ASSERT_EQUALS(s, "01");
    afl::string::putHexByte(s, 162);
    TS_ASSERT_EQUALS(s, "01A2");
    afl::string::putHexByte(s, 243, afl::string::HEX_DIGITS_LOWER);
    TS_ASSERT_EQUALS(s, "01A2f3");
}
