/**
  *  \file test/afl/checksums/sha224test.cpp
  *  \brief Test for afl::checksums::SHA224
  */

#include "afl/checksums/sha224.hpp"
#include "afl/test/testrunner.hpp"

#include "afl/base/countof.hpp"

namespace {
    struct TestCase {
        const char* m_string;
        uint32_t m_repeat;
        const uint8_t m_expect[28];
    };
}

using afl::base::ConstBytes_t;

/** Basic tests. */
AFL_TEST("afl.checksums.SHA224", a)
{
    afl::checksums::SHA224 cc;
    uint8_t bytes[28];

    // Querying an empty hash should return the expected result
    static const uint8_t expected[] = {
        0xd1, 0x4a, 0x02, 0x8c, 0x2a, 0x3a, 0x2b, 0xc9, 0x47, 0x61,
        0x02, 0xbb, 0x28, 0x82, 0x34, 0xc4, 0x15, 0xa2, 0xb0, 0x1f,
        0x82, 0x8e, 0xa6, 0x2a, 0xc5, 0xb3, 0xe4, 0x2f
    };
    ConstBytes_t result = cc.getHash(bytes);
    a.check("getHash empty 1", result.equalContent(expected));

    // Asking again should get the same result again
    result = cc.getHash(bytes);
    a.check("getHash empty 2", result.equalContent(expected));

    // Check string
    a.checkEqual("getHashAsHexString", cc.getHashAsHexString(), "d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f");

    // Asking a smaller buffer, must get correct content (first half of hash)
    uint8_t small[8];
    result = cc.getHash(small);
    a.checkEqual("getHash small", result.size(), 8U);
    a.checkEqualContent("getHash small content", ConstBytes_t(bytes).trim(8), ConstBytes_t(small));

    // Asking a larger buffer, must get correct content (just the hash)
    uint8_t large[64];
    result = cc.getHash(large);
    a.checkEqual("getHash large", result.size(), 28U);
    a.checkEqualContent("getHash large content", ConstBytes_t(bytes), ConstBytes_t(large).trim(28));

    // Self-information
    a.checkEqual("getHashSize 1", cc.getHashSize(), 28U);
    a.checkEqual("getHashSize 2", cc.getHashSize(), cc.HASH_SIZE);
    a.checkEqual("getBlockSize", cc.getBlockSize(), 64U);
}

/** Hash tests from RFC 6234. */
AFL_TEST("afl.checksums.SHA224:getHash:rfc6234", a)
{
    static const TestCase cases[] = {
        { "abc", 1,
          { 0x23, 0x09, 0x7D, 0x22, 0x34, 0x05, 0xD8, 0x22, 0x86, 0x42, 0xA4, 0x77, 0xBD, 0xA2, 0x55, 0xB3, 0x2A, 0xAD, 0xBC, 0xE4, 0xBD, 0xA0, 0xB3, 0xF7, 0xE3, 0x6C, 0x9D, 0xA7 } },
        { "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 1,
          { 0x75, 0x38, 0x8B, 0x16, 0x51, 0x27, 0x76, 0xCC, 0x5D, 0xBA, 0x5D, 0xA1, 0xFD, 0x89, 0x01, 0x50, 0xB0, 0xC6, 0x45, 0x5C, 0xB4, 0xF5, 0x8B, 0x19, 0x52, 0x52, 0x25, 0x25 } },
        { "a", 1000000,
          { 0x20, 0x79, 0x46, 0x55, 0x98, 0x0C, 0x91, 0xD8, 0xBB, 0xB4, 0xC1, 0xEA, 0x97, 0x61, 0x8A, 0x4B, 0xF0, 0x3F, 0x42, 0x58, 0x19, 0x48, 0xB2, 0xEE, 0x4E, 0xE7, 0xAD, 0x67 } },
        { "0123456701234567012345670123456701234567012345670123456701234567", 10,
          { 0x56, 0x7F, 0x69, 0xF1, 0x68, 0xCD, 0x78, 0x44, 0xE6, 0x52, 0x59, 0xCE, 0x65, 0x8F, 0xE7, 0xAA, 0xDF, 0xA2, 0x52, 0x16, 0xE6, 0x8E, 0xCA, 0x0E, 0xB7, 0xAB, 0x82, 0x62 } },
        { "\x07", 1,
          { 0x00, 0xEC, 0xD5, 0xF1, 0x38, 0x42, 0x2B, 0x8A, 0xD7, 0x4C, 0x97, 0x99, 0xFD, 0x82, 0x6C, 0x53, 0x1B, 0xAD, 0x2F, 0xCA, 0xBC, 0x74, 0x50, 0xBE, 0xE2, 0xAA, 0x8C, 0x2A } },
        { "\x18\x80\x40\x05\xdd\x4f\xbd\x15\x56\x29\x9d\x6f\x9d\x93\xdf\x62", 1,
          { 0xDF, 0x90, 0xD7, 0x8A, 0xA7, 0x88, 0x21, 0xC9, 0x9B, 0x40, 0xBA, 0x4C, 0x96, 0x69, 0x21, 0xAC, 0xCD, 0x8F, 0xFB, 0x1E, 0x98, 0xAC, 0x38, 0x8E, 0x56, 0x19, 0x1D, 0xB1 } },
    };


    for (size_t i = 0; i < countof(cases); ++i) {
        // Compute the hash
        afl::checksums::SHA224 cc;
        for (size_t j = 0; j < cases[i].m_repeat; ++j) {
            cc.add(afl::string::toBytes(cases[i].m_string));
        }

        uint8_t buffer[64];
        a.check("", cc.getHash(buffer).equalContent(cases[i].m_expect));
    }
}

/** HMAC tests from RFC 6234. */
AFL_TEST("afl.checksums.SHA224:computeHMAC:rfc6234", a)
{
    afl::checksums::SHA224 cc;

    // 1
    cc.computeHMAC(afl::string::toBytes("\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b"),
                   afl::string::toBytes("\x48\x69\x20\x54\x68\x65\x72\x65"));
    a.checkEqual("case 1", cc.getHashAsHexString(), "896fb1128abbdf196832107cd49df33f47b4b1169912ba4f53684b22");

    // 2
    cc.computeHMAC(afl::string::toBytes("\x4a\x65\x66\x65"),
                   afl::string::toBytes("\x77\x68\x61\x74\x20\x64\x6f\x20\x79\x61\x20\x77\x61\x6e\x74"
                                        "\x20\x66\x6f\x72\x20\x6e\x6f\x74\x68\x69\x6e\x67\x3f"));
    a.checkEqual("case 2", cc.getHashAsHexString(), "a30e01098bc6dbbf45690f3a7e9e6d0f8bbea2a39e6148008fd05e44");

    // 3
    cc.computeHMAC(afl::string::toBytes("\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
                                        "\xaa\xaa\xaa\xaa\xaa"),
                   afl::string::toBytes("\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
                                        "\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
                                        "\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
                                        "\xdd\xdd\xdd\xdd\xdd"));
    a.checkEqual("case 3", cc.getHashAsHexString(), "7fb3cb3588c6c1f6ffa9694d7d6ad2649365b0c1f65d69d1ec8333ea");

    // 4
    cc.computeHMAC(afl::string::toBytes("\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
                                        "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19"),
                   afl::string::toBytes("\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd"
                                        "\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd"
                                        "\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd"
                                        "\xcd\xcd\xcd\xcd\xcd"));
    a.checkEqual("case 4", cc.getHashAsHexString(), "6c11506874013cac6a2abc1bb382627cec6a90d86efc012de7afec5a");
}
