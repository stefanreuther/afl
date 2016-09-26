/**
  *  \file u/t_charset_base64.cpp
  *  \brief Test for afl::charset::Base64
  */

#include "afl/charset/base64.hpp"

#include "u/t_charset.hpp"

/** Simple tests. */
void
TestCharsetBase64::testIt()
{
    afl::charset::Base64 b64;

    // Empty
    TS_ASSERT_EQUALS(b64.encode(afl::string::toMemory("")), "");
    TS_ASSERT_EQUALS(b64.decode(afl::string::toMemory("")), "");

    // Single character
    TS_ASSERT_EQUALS(b64.encode(afl::string::toMemory("a")), "YQ==");
    TS_ASSERT_EQUALS(b64.decode(afl::string::toMemory("YQ==")), "a");
    TS_ASSERT_EQUALS(b64.decode(afl::string::toMemory("YQ")), "a");

    // Three characters
    TS_ASSERT_EQUALS(b64.encode(afl::string::toMemory("aaa")), "YWFh");
    TS_ASSERT_EQUALS(b64.decode(afl::string::toMemory("YWFh")), "aaa");
    TS_ASSERT_EQUALS(b64.encode(afl::string::toMemory("aaaaaaaaa")), "YWFhYWFhYWFh");
    TS_ASSERT_EQUALS(b64.decode(afl::string::toMemory("YWFhYWFhYWFh")), "aaaaaaaaa");

    // Test full 8-bit transparency (with 1-byte payload)
    for (uint32_t i = 0; i < 255; ++i) {
        char c[1] = { char(i) };

        String_t encoded = b64.encode(c);
        TS_ASSERT_EQUALS(encoded.size(), 4U);

        String_t decoded = b64.decode(afl::string::toMemory(encoded));
        TS_ASSERT_EQUALS(decoded.size(), sizeof(c));
        TS_ASSERT_SAME_DATA(decoded.data(), c, sizeof(c));
    }

    // Test full 8-bit transparency (with 2-byte payload)
    for (uint32_t i = 0; i < 65536; ++i) {
        char c[2] = { char(i & 255), char(i >> 8) };

        String_t encoded = b64.encode(c);
        TS_ASSERT_EQUALS(encoded.size(), 4U);

        String_t decoded = b64.decode(afl::string::toMemory(encoded));
        TS_ASSERT_EQUALS(decoded.size(), sizeof(c));
        TS_ASSERT_SAME_DATA(decoded.data(), c, sizeof(c));
    }

    // Test full 8-bit transparency (with 4-byte payload)
    for (uint32_t i = 0; i < 65536; ++i) {
        char c[4] = { char(242), char(i & 255), char(i >> 8), 17 };

        String_t encoded = b64.encode(c);
        TS_ASSERT_EQUALS(encoded.size(), 8U);

        String_t decoded = b64.decode(afl::string::toMemory(encoded));
        TS_ASSERT_EQUALS(decoded.size(), sizeof(c));
        TS_ASSERT_SAME_DATA(decoded.data(), c, sizeof(c));
    }
}
