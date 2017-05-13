/**
  *  \file u/t_charset_base64.cpp
  *  \brief Test for afl::charset::Base64
  */

#include <memory>
#include "afl/charset/base64.hpp"

#include "u/t_charset.hpp"

/** Simple tests. */
void
TestCharsetBase64::testIt()
{
    afl::charset::Base64 b64;

    // Empty
    TS_ASSERT_EQUALS(afl::string::fromBytes(b64.encode(afl::string::toMemory(""))), "");
    TS_ASSERT_EQUALS(b64.decode(afl::string::toBytes("")), "");

    // Single character
    TS_ASSERT_EQUALS(afl::string::fromBytes(b64.encode(afl::string::toMemory("a"))), "YQ==");
    TS_ASSERT_EQUALS(b64.decode(afl::string::toBytes("YQ==")), "a");
    TS_ASSERT_EQUALS(b64.decode(afl::string::toBytes("YQ")), "a");

    // Three characters
    TS_ASSERT_EQUALS(afl::string::fromBytes(b64.encode(afl::string::toMemory("aaa"))), "YWFh");
    TS_ASSERT_EQUALS(b64.decode(afl::string::toBytes("YWFh")), "aaa");
    TS_ASSERT_EQUALS(afl::string::fromBytes(b64.encode(afl::string::toMemory("aaaaaaaaa"))), "YWFhYWFhYWFh");
    TS_ASSERT_EQUALS(b64.decode(afl::string::toBytes("YWFhYWFhYWFh")), "aaaaaaaaa");

    // Test full 8-bit transparency (with 1-byte payload)
    for (uint32_t i = 0; i < 255; ++i) {
        char c[1] = { char(i) };

        afl::base::GrowableBytes_t encoded = b64.encode(c);
        TS_ASSERT_EQUALS(encoded.size(), 4U);

        String_t decoded = b64.decode(encoded);
        TS_ASSERT_EQUALS(decoded.size(), sizeof(c));
        TS_ASSERT_SAME_DATA(decoded.data(), c, sizeof(c));
    }

    // Test full 8-bit transparency (with 2-byte payload)
    for (uint32_t i = 0; i < 65536; ++i) {
        char c[2] = { char(i & 255), char(i >> 8) };

        afl::base::GrowableBytes_t encoded = b64.encode(c);
        TS_ASSERT_EQUALS(encoded.size(), 4U);

        String_t decoded = b64.decode(encoded);
        TS_ASSERT_EQUALS(decoded.size(), sizeof(c));
        TS_ASSERT_SAME_DATA(decoded.data(), c, sizeof(c));
    }

    // Test full 8-bit transparency (with 4-byte payload)
    for (uint32_t i = 0; i < 65536; ++i) {
        char c[4] = { char(242), char(i & 255), char(i >> 8), 17 };

        afl::base::GrowableBytes_t encoded = b64.encode(c);
        TS_ASSERT_EQUALS(encoded.size(), 8U);

        String_t decoded = b64.decode(encoded);
        TS_ASSERT_EQUALS(decoded.size(), sizeof(c));
        TS_ASSERT_SAME_DATA(decoded.data(), c, sizeof(c));
    }
}

/** Test clone. */
void
TestCharsetBase64::testClone()
{
    afl::charset::Base64 testee;
    std::auto_ptr<afl::charset::Charset> result(testee.clone());
    TS_ASSERT(result.get() != 0);
    TS_ASSERT(dynamic_cast<afl::charset::Base64*>(result.get()) != 0);
}

/** Test fallback on error. */
void
TestCharsetBase64::testError()
{
    afl::charset::Base64 b64;

    TS_ASSERT_EQUALS(b64.decode(afl::string::toBytes("YQ")), "a");
    TS_ASSERT_EQUALS(b64.decode(afl::string::toBytes("YQA")),  String_t("a\0",   2));  // correct
    TS_ASSERT_EQUALS(b64.decode(afl::string::toBytes("YQAA")), String_t("a\0\0", 3));  // correct
    TS_ASSERT_EQUALS(b64.decode(afl::string::toBytes("YQ$")),  String_t("a\0",   2));  // incorrect
    TS_ASSERT_EQUALS(b64.decode(afl::string::toBytes("YQ$$")), String_t("a\0\0", 3));  // incorrect
}
