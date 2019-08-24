/**
  *  \file u/t_charset_asciitransliterator.cpp
  *  \brief Test for afl::charset::AsciiTransliterator
  */

#include "afl/charset/asciitransliterator.hpp"

#include "t_charset.hpp"
#include "afl/charset/utf8.hpp"

void
TestCharsetAsciiTransliterator::testSimple()
{
    afl::charset::AsciiTransliterator testee;

    // Encode
    afl::base::GrowableBytes_t g = testee.encode(afl::string::toMemory("hihi"));
    TS_ASSERT_EQUALS(afl::string::fromBytes(g), "hihi");

    // Decode
    TS_ASSERT_EQUALS(testee.decode(afl::string::toBytes("ha")), "ha");
    TS_ASSERT_EQUALS(testee.decode(afl::string::toBytes("bl""\xc3\xb6""d")), "blod");
    TS_ASSERT_EQUALS(testee.decode(afl::string::toBytes("\xc8\xa2")), "OU");
    TS_ASSERT_EQUALS(testee.decode(afl::string::toBytes("\xd0\xaf\xd0\xa9\xd0\xa3\xd0\xa0")), "IaShchUR");
}

void
TestCharsetAsciiTransliterator::testCoverage()
{
    afl::charset::AsciiTransliterator testee;

    // Count the characters we have transliterations for.
    // Should be well over 1000.
    // If this fails, there's a problem with our binary search.
    int count = 0;
    for (afl::charset::Unichar_t i = 0; i < 0x3000; ++i) {
        String_t s;
        afl::charset::Utf8().append(s, i);
        if (!testee.decode(afl::string::toBytes(s)).empty()) {
            ++count;
        }
    }
    TS_ASSERT_LESS_THAN_EQUALS(1000, count);
}

