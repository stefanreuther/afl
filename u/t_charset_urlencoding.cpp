/**
  *  \file u/t_charset_urlencoding.cpp
  *  \brief Test for afl::charset::UrlEncoding
  */

#include <memory>
#include "afl/charset/urlencoding.hpp"

#include "u/t_charset.hpp"

/** Simple test. */
void
TestCharsetUrlEncoding::testIt()
{
    afl::charset::UrlEncoding ue;

    // Empty
    TS_ASSERT_EQUALS(afl::string::fromBytes(ue.encode(afl::string::toMemory(""))), "");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toBytes("")), "");

    // ASCII
    TS_ASSERT_EQUALS(afl::string::fromBytes(ue.encode(afl::string::toMemory("abc"))), "abc");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toBytes("abc")), "abc");

    // UTF-8
    TS_ASSERT_EQUALS(afl::string::fromBytes(ue.encode(afl::string::toMemory("a\xC2\x85"))), "a%C2%85");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toBytes("a%C2%85")), "a\xC2\x85");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toBytes("a%c2%85")), "a\xC2\x85");

    // ASCII with spaces
    TS_ASSERT_EQUALS(afl::string::fromBytes(ue.encode(afl::string::toMemory("a b c"))), "a+b+c");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toBytes("a+b+c")), "a b c");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toBytes("a%20b%20c")), "a b c");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toBytes("a b c")), "a b c");

    // ASCII with equals
    TS_ASSERT_EQUALS(afl::string::fromBytes(ue.encode(afl::string::toMemory("a=c"))), "a%3Dc");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toBytes("a%3Dc")), "a=c");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toBytes("a%3dc")), "a=c");

    // ASCII with plus
    TS_ASSERT_EQUALS(afl::string::fromBytes(ue.encode(afl::string::toMemory("a+=c"))), "a%2B%3Dc");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toBytes("a%2B%3Dc")), "a+=c");

    // ASCII with ampersand
    TS_ASSERT_EQUALS(afl::string::fromBytes(ue.encode(afl::string::toMemory("a&c"))), "a%26c");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toBytes("a%26c")), "a&c");

    // Non-UTF-8
    TS_ASSERT_EQUALS(afl::string::fromBytes(ue.encode(afl::string::toMemory("a\xAA\xBBx"))), "a%AA%BBx");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toBytes("a%AA%BBx")), "a\xAA\xBBx");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toBytes("a%aa%bbx")), "a\xAA\xBBx");

    // Errors
    TS_ASSERT_EQUALS(ue.decode(afl::string::toBytes("%")), "%");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toBytes("%1")), "%1");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toBytes("%XY")), "%XY");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toBytes("%3X")), "%3X");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toBytes("%%33")), "%%33");
    TS_ASSERT_EQUALS(ue.decode(afl::string::toBytes("%%%%33")), "%%%3");  // fourth "=" starts a character
}

/** Test clone. */
void
TestCharsetUrlEncoding::testClone()
{
    afl::charset::UrlEncoding testee;
    std::auto_ptr<afl::charset::Charset> result(testee.clone());
    TS_ASSERT(result.get() != 0);
    TS_ASSERT(dynamic_cast<afl::charset::UrlEncoding*>(result.get()) != 0);
}

