/**
  *  \file test/afl/charset/codepagecharsettest.cpp
  *  \brief Test for afl::charset::CodepageCharset
  */

#include "afl/charset/codepagecharset.hpp"
#include "afl/test/testrunner.hpp"

#include "afl/charset/codepage.hpp"

/** Simple test. */
AFL_TEST("afl.charset.CodepageCharset:encode+decode", a)
{
    afl::charset::CodepageCharset ccs(afl::charset::g_codepage437);
    a.checkEqual("01", &ccs.get(), &afl::charset::g_codepage437);

    // Trivial cases
    a.checkEqual("11", afl::string::fromBytes(ccs.encode(afl::string::ConstStringMemory_t())), "");
    a.checkEqual("12", ccs.decode(afl::base::ConstBytes_t()), "");
    a.checkEqual("13", afl::string::fromBytes(ccs.encode(afl::string::toMemory(" "))), " ");
    a.checkEqual("14", ccs.decode(afl::string::toBytes(" ")), " ");
    a.checkEqual("15", afl::string::fromBytes(ccs.encode(afl::string::toMemory("\x7F"))), "\x7F");
    a.checkEqual("16", ccs.decode(afl::string::toBytes("\x7F")), "\x7F");

    // Decode
    a.checkEqual("21", ccs.decode(afl::string::toBytes("\x80")), "\xC3\x87");   // U+00C7, C with cedille
    a.checkEqual("22", ccs.decode(afl::string::toBytes("\xFF")), "\xC2\xA0");   // U+00A0, nbsp
    a.checkEqual("23",
                 ccs.decode(afl::string::toBytes("\xC2\x82")),
                 "\xE2\x94\xAC\xC3\xA9");         // U+252C box drawings horizontal+top, U+00E9 E-acute
    a.checkEqual("24", ccs.decode(afl::string::toBytes("a\x80z")), "a\xC3\x87z");

    // Encode
    a.checkEqual("31", afl::string::fromBytes(ccs.encode(afl::string::toMemory("\xC3\x87"))), "\x80");
    a.checkEqual("32", afl::string::fromBytes(ccs.encode(afl::string::toMemory("\xC2\xA0"))), "\xFF");
    a.checkEqual("33", afl::string::fromBytes(ccs.encode(afl::string::toMemory("\xE2\x94\xAC\xC3\xA9"))), "\xC2\x82");
    a.checkEqual("34", afl::string::fromBytes(ccs.encode(afl::string::toMemory("a\xC3\x87z"))), "a\x80z");

    // Encode some invalid characters
    a.checkEqual("41", afl::string::fromBytes(ccs.encode(afl::string::toMemory("\xC2\x80"))), "");       // U+0080, a control character
    a.checkEqual("42", afl::string::fromBytes(ccs.encode(afl::string::toMemory("a\xC2\x80z"))), "az");
    a.checkEqual("43", afl::string::fromBytes(ccs.encode(afl::string::toMemory("\xD0\x90"))), "");       // U+0410, cyrillic A
    a.checkEqual("44", afl::string::fromBytes(ccs.encode(afl::string::toMemory("a\xD0\x90z"))), "az");   // U+0410, cyrillic A
}

/** Test clone. */
AFL_TEST("afl.charset.CodepageCharset:clone", a)
{
    afl::charset::CodepageCharset testee(afl::charset::g_codepage437);

    std::auto_ptr<afl::charset::Charset> result(testee.clone());
    a.checkNonNull("01", result.get());
    a.checkNonNull("02", dynamic_cast<afl::charset::CodepageCharset*>(result.get()));

    // Verify that copy and original work
    a.checkEqual("11", testee.decode(afl::string::toBytes("\x80")), "\xC3\x87");   // U+00C7, C with cedille
    a.checkEqual("12", result->decode(afl::string::toBytes("\x80")), "\xC3\x87");   // U+00C7, C with cedille
    a.checkEqual("13", testee.decode(afl::string::toBytes("\xFF")), "\xC2\xA0");   // U+00A0, nbsp
    a.checkEqual("14", result->decode(afl::string::toBytes("\xFF")), "\xC2\xA0");   // U+00A0, nbsp
}
