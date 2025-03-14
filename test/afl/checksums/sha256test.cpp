/**
  *  \file test/afl/checksums/sha256test.cpp
  *  \brief Test for afl::checksums::SHA256
  */

#include "afl/checksums/sha256.hpp"
#include "afl/test/testrunner.hpp"

#include "afl/base/countof.hpp"

using afl::base::ConstBytes_t;

namespace {
    struct TestCase {
        const char* m_string;
        uint32_t m_repeat;
        const uint8_t m_expect[32];
    };
}

/** Basic tests. */
AFL_TEST("afl.checksums.SHA256", a)
{
    afl::checksums::SHA256 cc;
    uint8_t bytes[32];

    // Querying an empty hash should return the expected result
    static const uint8_t expected[] = {
        0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14, 0x9a, 0xfb,
        0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24, 0x27, 0xae, 0x41, 0xe4,
        0x64, 0x9b, 0x93, 0x4c, 0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52,
        0xb8, 0x55,
    };
    ConstBytes_t result = cc.getHash(bytes);
    a.check("empty 1", result.equalContent(expected));

    // Asking again should get the same result again
    result = cc.getHash(bytes);
    a.check("empty 2", result.equalContent(expected));

    // Check string
    a.checkEqual("getHashAsHexString", cc.getHashAsHexString(), "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");

    // Asking a smaller buffer, must get correct content (first half of hash)
    uint8_t small[8];
    result = cc.getHash(small);
    a.checkEqual("getHash small", result.size(), 8U);
    a.checkEqualContent("getHash small content", ConstBytes_t(bytes).trim(8), ConstBytes_t(small));

    // Asking a larger buffer, must get correct content (just the hash)
    uint8_t large[64];
    result = cc.getHash(large);
    a.checkEqual("getHash large", result.size(), 32U);
    a.checkEqualContent("getHash large content", ConstBytes_t(bytes), ConstBytes_t(large).trim(32));

    // Self-information
    a.checkEqual("getHashSize 1", cc.getHashSize(), 32U);
    a.checkEqual("getHashSize 2", cc.getHashSize(), cc.HASH_SIZE);
    a.checkEqual("getBlockSize", cc.getBlockSize(), 64U);
}

/** Hash tests from RFC 6234. */
AFL_TEST("afl.checksums.SHA256:getHash:rfc6234", a)
{
    static const TestCase cases[] = {
        { "abc", 1,
          { 0xBA, 0x78, 0x16, 0xBF, 0x8F, 0x01, 0xCF, 0xEA, 0x41, 0x41, 0x40, 0xDE, 0x5D, 0xAE, 0x22, 0x23,
            0xB0, 0x03, 0x61, 0xA3, 0x96, 0x17, 0x7A, 0x9C, 0xB4, 0x10, 0xFF, 0x61, 0xF2, 0x00, 0x15, 0xAD } },
        { "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 1,
          { 0x24, 0x8D, 0x6A, 0x61, 0xD2, 0x06, 0x38, 0xB8, 0xE5, 0xC0, 0x26, 0x93, 0x0C, 0x3E, 0x60, 0x39,
            0xA3, 0x3C, 0xE4, 0x59, 0x64, 0xFF, 0x21, 0x67, 0xF6, 0xEC, 0xED, 0xD4, 0x19, 0xDB, 0x06, 0xC1 } },
        { "a", 1000000,
          { 0xCD, 0xC7, 0x6E, 0x5C, 0x99, 0x14, 0xFB, 0x92, 0x81, 0xA1, 0xC7, 0xE2, 0x84, 0xD7, 0x3E, 0x67,
            0xF1, 0x80, 0x9A, 0x48, 0xA4, 0x97, 0x20, 0x0E, 0x04, 0x6D, 0x39, 0xCC, 0xC7, 0x11, 0x2C, 0xD0 } },
        { "0123456701234567012345670123456701234567012345670123456701234567", 10,
          { 0x59, 0x48, 0x47, 0x32, 0x84, 0x51, 0xBD, 0xFA, 0x85, 0x05, 0x62, 0x25, 0x46, 0x2C, 0xC1, 0xD8,
            0x67, 0xD8, 0x77, 0xFB, 0x38, 0x8D, 0xF0, 0xCE, 0x35, 0xF2, 0x5A, 0xB5, 0x56, 0x2B, 0xFB, 0xB5 } },
        { "\x19", 1,
          { 0x68, 0xAA, 0x2E, 0x2E, 0xE5, 0xDF, 0xF9, 0x6E, 0x33, 0x55, 0xE6, 0xC7, 0xEE, 0x37, 0x3E, 0x3D,
            0x6A, 0x4E, 0x17, 0xF7, 0x5F, 0x95, 0x18, 0xD8, 0x43, 0x70, 0x9C, 0x0C, 0x9B, 0xC3, 0xE3, 0xD4 } },
        { "\xe3\xd7\x25\x70\xdc\xdd\x78\x7c\xe3\x88\x7a\xb2\xcd\x68\x46\x52", 1,
          { 0x17, 0x5E, 0xE6, 0x9B, 0x02, 0xBA, 0x9B, 0x58, 0xE2, 0xB0, 0xA5, 0xFD, 0x13, 0x81, 0x9C, 0xEA,
            0x57, 0x3F, 0x39, 0x40, 0xA9, 0x4F, 0x82, 0x51, 0x28, 0xCF, 0x42, 0x09, 0xBE, 0xAB, 0xB4, 0xE8 } },
    };

    for (size_t i = 0; i < countof(cases); ++i) {
        // Compute the hash
        afl::checksums::SHA256 cc;
        for (size_t j = 0; j < cases[i].m_repeat; ++j) {
            cc.add(afl::string::toBytes(cases[i].m_string));
        }

        uint8_t buffer[64];
        a.check("", cc.getHash(buffer).equalContent(cases[i].m_expect));
    }
}

/** HMAC tests from RFC 6234. */
AFL_TEST("afl.checksums.SHA256:computeHMAC:rfc6234", a)
{
    afl::checksums::SHA256 cc;

    // 1
    cc.computeHMAC(afl::string::toBytes("\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b"),
                   afl::string::toBytes("\x48\x69\x20\x54\x68\x65\x72\x65"));
    a.checkEqual("case 1", cc.getHashAsHexString(), "b0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7");

    // 2
    cc.computeHMAC(afl::string::toBytes("\x4a\x65\x66\x65"),
                   afl::string::toBytes("\x77\x68\x61\x74\x20\x64\x6f\x20\x79\x61\x20\x77\x61\x6e\x74"
                                        "\x20\x66\x6f\x72\x20\x6e\x6f\x74\x68\x69\x6e\x67\x3f"));
    a.checkEqual("case 2", cc.getHashAsHexString(), "5bdcc146bf60754e6a042426089575c75a003f089d2739839dec58b964ec3843");

    // 3
    cc.computeHMAC(afl::string::toBytes("\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
                                        "\xaa\xaa\xaa\xaa\xaa"),
                   afl::string::toBytes("\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
                                        "\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
                                        "\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
                                        "\xdd\xdd\xdd\xdd\xdd"));
    a.checkEqual("case 3", cc.getHashAsHexString(), "773ea91e36800e46854db8ebd09181a72959098b3ef8c122d9635514ced565fe");

    // 4
    cc.computeHMAC(afl::string::toBytes("\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
                                        "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19"),
                   afl::string::toBytes("\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd"
                                        "\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd"
                                        "\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd"
                                        "\xcd\xcd\xcd\xcd\xcd"));
    a.checkEqual("case 4", cc.getHashAsHexString(), "82558a389a443c0ea4cc819899f2083a85f0faa3e578f8077a2e3ff46729665b");
}
