/**
  *  \file u/t_charset_codepagecharset.cpp
  *  \brief Test for afl::charset::CodepageCharset
  */

#include <memory>
#include "afl/charset/codepagecharset.hpp"

#include "u/t_charset.hpp"
#include "afl/charset/codepage.hpp"

/** Simple test. */
void
TestCharsetCodepageCharset::testIt()
{
    afl::charset::CodepageCharset ccs(afl::charset::g_codepage437);

    // Trivial cases
    TS_ASSERT_EQUALS(afl::string::fromBytes(ccs.encode(afl::string::ConstStringMemory_t())), "");
    TS_ASSERT_EQUALS(ccs.decode(afl::base::ConstBytes_t()), "");
    TS_ASSERT_EQUALS(afl::string::fromBytes(ccs.encode(afl::string::toMemory(" "))), " ");
    TS_ASSERT_EQUALS(ccs.decode(afl::string::toBytes(" ")), " ");
    TS_ASSERT_EQUALS(afl::string::fromBytes(ccs.encode(afl::string::toMemory("\x7F"))), "\x7F");
    TS_ASSERT_EQUALS(ccs.decode(afl::string::toBytes("\x7F")), "\x7F");

    // Decode
    TS_ASSERT_EQUALS(ccs.decode(afl::string::toBytes("\x80")), "\xC3\x87");   // U+00C7, C with cedille
    TS_ASSERT_EQUALS(ccs.decode(afl::string::toBytes("\xFF")), "\xC2\xA0");   // U+00A0, nbsp
    TS_ASSERT_EQUALS(ccs.decode(afl::string::toBytes("\xC2\x82")),
                     "\xE2\x94\xAC\xC3\xA9");         // U+252C box drawings horizontal+top, U+00E9 E-acute
    TS_ASSERT_EQUALS(ccs.decode(afl::string::toBytes("a\x80z")), "a\xC3\x87z");

    // Encode
    TS_ASSERT_EQUALS(afl::string::fromBytes(ccs.encode(afl::string::toMemory("\xC3\x87"))), "\x80");
    TS_ASSERT_EQUALS(afl::string::fromBytes(ccs.encode(afl::string::toMemory("\xC2\xA0"))), "\xFF");
    TS_ASSERT_EQUALS(afl::string::fromBytes(ccs.encode(afl::string::toMemory("\xE2\x94\xAC\xC3\xA9"))), "\xC2\x82");
    TS_ASSERT_EQUALS(afl::string::fromBytes(ccs.encode(afl::string::toMemory("a\xC3\x87z"))), "a\x80z");

    // Encode some invalid characters
    TS_ASSERT_EQUALS(afl::string::fromBytes(ccs.encode(afl::string::toMemory("\xC2\x80"))), "");       // U+0080, a control character
    TS_ASSERT_EQUALS(afl::string::fromBytes(ccs.encode(afl::string::toMemory("a\xC2\x80z"))), "az");
    TS_ASSERT_EQUALS(afl::string::fromBytes(ccs.encode(afl::string::toMemory("\xD0\x90"))), "");       // U+0410, cyrillic A
    TS_ASSERT_EQUALS(afl::string::fromBytes(ccs.encode(afl::string::toMemory("a\xD0\x90z"))), "az");   // U+0410, cyrillic A
}

/** Test clone. */
void
TestCharsetCodepageCharset::testClone()
{
    afl::charset::CodepageCharset testee(afl::charset::g_codepage437);

    std::auto_ptr<afl::charset::Charset> result(testee.clone());
    TS_ASSERT(result.get() != 0);
    TS_ASSERT(dynamic_cast<afl::charset::CodepageCharset*>(result.get()) != 0);

    // Verify that copy and original work
    TS_ASSERT_EQUALS(testee.decode(afl::string::toBytes("\x80")), "\xC3\x87");   // U+00C7, C with cedille
    TS_ASSERT_EQUALS(result->decode(afl::string::toBytes("\x80")), "\xC3\x87");   // U+00C7, C with cedille
    TS_ASSERT_EQUALS(testee.decode(afl::string::toBytes("\xFF")), "\xC2\xA0");   // U+00A0, nbsp
    TS_ASSERT_EQUALS(result->decode(afl::string::toBytes("\xFF")), "\xC2\xA0");   // U+00A0, nbsp
}
