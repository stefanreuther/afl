/**
  *  \file u/t_charset_utf8charset.cpp
  *  \brief Test for afl::charset::Utf8Charset
  */

#include "afl/charset/utf8charset.hpp"

#include "u/t_charset.hpp"

/** Simple test. */
void
TestCharsetUtf8Charset::testIt()
{
    afl::charset::Utf8Charset u8cs;

    TS_ASSERT_EQUALS(u8cs.decode(afl::string::toMemory("foo")), "foo");
    TS_ASSERT_EQUALS(u8cs.decode(afl::string::toMemory("f\xc2\x90")), "f\xc2\x90");

    TS_ASSERT_EQUALS(u8cs.encode(afl::string::toMemory("foo")), "foo");
    TS_ASSERT_EQUALS(u8cs.encode(afl::string::toMemory("f\xc2\x90")), "f\xc2\x90");
}
