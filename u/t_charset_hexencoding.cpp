/**
  *  \file u/t_charset_hexencoding.cpp
  *  \brief Test for afl::charset::HexEncoding
  */

#include <memory>
#include "afl/charset/hexencoding.hpp"

#include "t_charset.hpp"
#include "afl/string/hex.hpp"

/** Simple test. */
void
TestCharsetHexEncoding::testIt()
{
    // Standard constructor
    {
        afl::charset::HexEncoding testee;
        TS_ASSERT_EQUALS(afl::string::fromBytes(testee.encode(afl::string::toMemory("aB?"))), "61423F");
        TS_ASSERT_EQUALS(testee.decode(afl::string::toBytes("61423F")), "aB?");
        TS_ASSERT_EQUALS(testee.decode(afl::string::toBytes("61423f")), "aB?");

        // We can leave spaces for decoding (currently undocumented feature)
        TS_ASSERT_EQUALS(testee.decode(afl::string::toBytes("61 42 3f")), "aB?");

        // Clone
        std::auto_ptr<afl::charset::HexEncoding> clone(testee.clone());
        TS_ASSERT(clone.get() != 0);
        TS_ASSERT_EQUALS(afl::string::fromBytes(clone->encode(afl::string::toMemory("aB?"))), "61423F");
        TS_ASSERT_EQUALS(clone->decode(afl::string::toBytes("61423F")), "aB?");
    }

    // Specific alphabet
    {
        afl::charset::HexEncoding testee(afl::string::HEX_DIGITS_LOWER);
        TS_ASSERT_EQUALS(afl::string::fromBytes(testee.encode(afl::string::toMemory("aB?"))), "61423f");

        // Clone
        std::auto_ptr<afl::charset::HexEncoding> clone(testee.clone());
        TS_ASSERT(clone.get() != 0);
        TS_ASSERT_EQUALS(afl::string::fromBytes(clone->encode(afl::string::toMemory("aB?"))), "61423f");
        TS_ASSERT_EQUALS(clone->decode(afl::string::toBytes("61423f")), "aB?");
    }
}
