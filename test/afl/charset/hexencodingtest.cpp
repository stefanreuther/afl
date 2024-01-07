/**
  *  \file test/afl/charset/hexencodingtest.cpp
  *  \brief Test for afl::charset::HexEncoding
  */

#include "afl/charset/hexencoding.hpp"
#include "afl/test/testrunner.hpp"

#include "afl/string/hex.hpp"

// Standard constructor
AFL_TEST("afl.charset.HexEncoding:default", a)
{
    afl::charset::HexEncoding testee;
    a.checkEqual("01", afl::string::fromBytes(testee.encode(afl::string::toMemory("aB?"))), "61423F");
    a.checkEqual("02", testee.decode(afl::string::toBytes("61423F")), "aB?");
    a.checkEqual("03", testee.decode(afl::string::toBytes("61423f")), "aB?");

    // We can leave spaces for decoding (currently undocumented feature)
    a.checkEqual("21", testee.decode(afl::string::toBytes("61 42 3f")), "aB?");

    // Clone
    std::auto_ptr<afl::charset::HexEncoding> clone(testee.clone());
    a.checkNonNull("31", clone.get());
    a.checkEqual("32", afl::string::fromBytes(clone->encode(afl::string::toMemory("aB?"))), "61423F");
    a.checkEqual("33", clone->decode(afl::string::toBytes("61423F")), "aB?");
}

// Specific alphabet
AFL_TEST("afl.charset.HexEncoding:specific", a)
{
    afl::charset::HexEncoding testee(afl::string::HEX_DIGITS_LOWER);
    a.checkEqual("01", afl::string::fromBytes(testee.encode(afl::string::toMemory("aB?"))), "61423f");

    // Clone
    std::auto_ptr<afl::charset::HexEncoding> clone(testee.clone());
    a.checkNonNull("11", clone.get());
    a.checkEqual("12", afl::string::fromBytes(clone->encode(afl::string::toMemory("aB?"))), "61423f");
    a.checkEqual("13", clone->decode(afl::string::toBytes("61423f")), "aB?");
}
