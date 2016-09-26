/**
  *  \file u/t_charset_urlencoding.cpp
  *  \brief Test for afl::charset::UrlEncoding
  */

#include "afl/charset/urlencoding.hpp"

#include "u/t_charset.hpp"

/** Simple test. */
void
TestCharsetUrlEncoding::testIt()
{
    afl::charset::UrlEncoding ue;

    // Empty
    TS_ASSERT_EQUALS(ue.encode(afl::string::toMemory("")), "");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toMemory("")), "");

    // ASCII
    TS_ASSERT_EQUALS(ue.encode(afl::string::toMemory("abc")), "abc");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toMemory("abc")), "abc");

    // UTF-8
    TS_ASSERT_EQUALS(ue.encode(afl::string::toMemory("a\xC2\x85")), "a%C2%85");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toMemory("a%C2%85")), "a\xC2\x85");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toMemory("a%c2%85")), "a\xC2\x85");

    // ASCII with spaces
    TS_ASSERT_EQUALS(ue.encode(afl::string::toMemory("a b c")), "a+b+c");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toMemory("a+b+c")), "a b c");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toMemory("a%20b%20c")), "a b c");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toMemory("a b c")), "a b c");

    // ASCII with equals
    TS_ASSERT_EQUALS(ue.encode(afl::string::toMemory("a=c")), "a%3Dc");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toMemory("a%3Dc")), "a=c");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toMemory("a%3dc")), "a=c");

    // ASCII with plus
    TS_ASSERT_EQUALS(ue.encode(afl::string::toMemory("a+=c")), "a%2B%3Dc");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toMemory("a%2B%3Dc")), "a+=c");

    // ASCII with ampersand
    TS_ASSERT_EQUALS(ue.encode(afl::string::toMemory("a&c")), "a%26c");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toMemory("a%26c")), "a&c");

    // Non-UTF-8
    TS_ASSERT_EQUALS(ue.encode(afl::string::toMemory("a\xAA\xBBx")), "a%AA%BBx");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toMemory("a%AA%BBx")), "a\xAA\xBBx");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toMemory("a%aa%bbx")), "a\xAA\xBBx");

    // Errors
    TS_ASSERT_EQUALS(ue.decode(afl::string::toMemory("%")), "%");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toMemory("%1")), "%1");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toMemory("%XY")), "%XY");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toMemory("%3X")), "%3X");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toMemory("%%33")), "%%33");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toMemory("%%%%33")), "%%%3");  // fourth "=" starts a character
}
