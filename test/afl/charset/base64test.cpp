/**
  *  \file test/afl/charset/base64test.cpp
  *  \brief Test for afl::charset::Base64
  */

#include "afl/charset/base64.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.charset.Base64:basic", a) {
    afl::charset::Base64 b64;

    // Empty
    a.checkEqual("01", afl::string::fromBytes(b64.encode(afl::string::toMemory(""))), "");
    a.checkEqual("02", b64.decode(afl::string::toBytes("")), "");

    // Single character
    a.checkEqual("11", afl::string::fromBytes(b64.encode(afl::string::toMemory("a"))), "YQ==");
    a.checkEqual("12", b64.decode(afl::string::toBytes("YQ==")), "a");
    a.checkEqual("13", b64.decode(afl::string::toBytes("YQ")), "a");

    // Three characters
    a.checkEqual("21", afl::string::fromBytes(b64.encode(afl::string::toMemory("aaa"))), "YWFh");
    a.checkEqual("22", b64.decode(afl::string::toBytes("YWFh")), "aaa");
    a.checkEqual("23", afl::string::fromBytes(b64.encode(afl::string::toMemory("aaaaaaaaa"))), "YWFhYWFhYWFh");
    a.checkEqual("24", b64.decode(afl::string::toBytes("YWFhYWFhYWFh")), "aaaaaaaaa");
}

// Test full 8-bit transparency (with 1-byte payload)
AFL_TEST("afl.charset.Base64:transparency:1", a) {
    afl::charset::Base64 b64;
    for (uint32_t i = 0; i < 255; ++i) {
        const char c[1] = { char(i) };

        afl::base::GrowableBytes_t encoded = b64.encode(c);
        a.checkEqual("encode", encoded.size(), 4U);

        String_t decoded = b64.decode(encoded);
        a.checkEqual("decoded size", decoded.size(), sizeof(c));
        a.checkEqualContent("decoded content", afl::string::toBytes(decoded), afl::base::fromObject(c));
    }
}

// Test full 8-bit transparency (with 2-byte payload)
AFL_TEST("afl.charset.Base64:transparency:2", a) {
    afl::charset::Base64 b64;
    for (uint32_t i = 0; i < 65536; ++i) {
        const char c[2] = { char(i & 255), char(i >> 8) };

        afl::base::GrowableBytes_t encoded = b64.encode(c);
        a.checkEqual("encode", encoded.size(), 4U);

        String_t decoded = b64.decode(encoded);
        a.checkEqual("decoded size", decoded.size(), sizeof(c));
        a.checkEqualContent("decoded content", afl::string::toBytes(decoded), afl::base::fromObject(c));
    }
}

// Test full 8-bit transparency (with 4-byte payload)
AFL_TEST("afl.charset.Base64:transparency:4", a) {
    afl::charset::Base64 b64;
    for (uint32_t i = 0; i < 65536; ++i) {
        const char c[4] = { char(242), char(i & 255), char(i >> 8), 17 };

        afl::base::GrowableBytes_t encoded = b64.encode(c);
        a.checkEqual("encode", encoded.size(), 8U);

        String_t decoded = b64.decode(encoded);
        a.checkEqual("decoded size", decoded.size(), sizeof(c));
        a.checkEqualContent("decoded content", afl::string::toBytes(decoded), afl::base::fromObject(c));
    }
}

/** Test clone. */
AFL_TEST("afl.charset.Base64:clone", a) {
    afl::charset::Base64 testee;
    std::auto_ptr<afl::charset::Charset> result(testee.clone());
    a.checkNonNull("result", result.get());
    a.check("type", dynamic_cast<afl::charset::Base64*>(result.get()) != 0);
}

/** Test fallback on error. */
AFL_TEST("afl.charset.Base64:error", a) {
    afl::charset::Base64 b64;

    a.checkEqual("01", b64.decode(afl::string::toBytes("YQ")), "a");
    a.checkEqual("02", b64.decode(afl::string::toBytes("YQA")),  String_t("a\0",   2));  // correct
    a.checkEqual("03", b64.decode(afl::string::toBytes("YQAA")), String_t("a\0\0", 3));  // correct
    a.checkEqual("04", b64.decode(afl::string::toBytes("YQ$")),  String_t("a\0",   2));  // incorrect
    a.checkEqual("05", b64.decode(afl::string::toBytes("YQ$$")), String_t("a\0\0", 3));  // incorrect
}
