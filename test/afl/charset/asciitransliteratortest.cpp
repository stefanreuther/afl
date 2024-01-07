/**
  *  \file test/afl/charset/asciitransliteratortest.cpp
  *  \brief Test for afl::charset::AsciiTransliterator
  */

#include "afl/charset/asciitransliterator.hpp"
#include "afl/test/testrunner.hpp"

#include "afl/charset/utf8.hpp"

AFL_TEST("afl.charset.AsciiTransliterator:encode+decode", a)
{
    afl::charset::AsciiTransliterator testee;

    // Encode
    afl::base::GrowableBytes_t g = testee.encode(afl::string::toMemory("hihi"));
    a.checkEqual("encode", afl::string::fromBytes(g), "hihi");

    // Decode
    a.checkEqual("decode 1", testee.decode(afl::string::toBytes("ha")), "ha");
    a.checkEqual("decode 2", testee.decode(afl::string::toBytes("bl""\xc3\xb6""d")), "blod");
    a.checkEqual("decode 3", testee.decode(afl::string::toBytes("\xc8\xa2")), "OU");
    a.checkEqual("decode 4", testee.decode(afl::string::toBytes("\xd0\xaf\xd0\xa9\xd0\xa3\xd0\xa0")), "IaShchUR");
}

AFL_TEST("afl.charset.AsciiTransliterator:coverage", a)
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
    a.check("", 1000 < count);
}

