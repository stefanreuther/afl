/**
  *  \file test/afl/checksums/sha384test.cpp
  *  \brief Test for afl::checksums::SHA384
  */

#include "afl/checksums/sha384.hpp"
#include "afl/test/testrunner.hpp"

#include "afl/base/countof.hpp"

using afl::base::ConstBytes_t;

namespace {
    struct TestCase {
        const char* m_string;
        uint32_t m_repeat;
        const uint8_t m_expect[48];
    };
}

/** Basic tests. */
AFL_TEST("afl.checksums.SHA384", a)
{
    afl::checksums::SHA384 cc;
    uint8_t bytes[48];

    // Querying an empty hash should return the expected result
    static const uint8_t expected[] = {
        0x38, 0xb0, 0x60, 0xa7, 0x51, 0xac, 0x96, 0x38, 0x4c, 0xd9,
        0x32, 0x7e, 0xb1, 0xb1, 0xe3, 0x6a, 0x21, 0xfd, 0xb7, 0x11,
        0x14, 0xbe, 0x07, 0x43, 0x4c, 0x0c, 0xc7, 0xbf, 0x63, 0xf6,
        0xe1, 0xda, 0x27, 0x4e, 0xde, 0xbf, 0xe7, 0x6f, 0x65, 0xfb,
        0xd5, 0x1a, 0xd2, 0xf1, 0x48, 0x98, 0xb9, 0x5b,
    };
    ConstBytes_t result = cc.getHash(bytes);
    a.check("getHash empty 1", result.equalContent(expected));

    // Asking again should get the same result again
    result = cc.getHash(bytes);
    a.check("getHash empty 2", result.equalContent(expected));

    // Check string
    a.checkEqual("getHashAsHexString", cc.getHashAsHexString(), "38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b");

    // Asking a smaller buffer, must get correct content (first half of hash)
    uint8_t small[8];
    result = cc.getHash(small);
    a.checkEqual("getHash small", result.size(), 8U);
    a.checkEqualContent("getHash small content", ConstBytes_t(bytes).trim(8), ConstBytes_t(small));

    // Asking a larger buffer, must get correct content (just the hash)
    uint8_t large[64];
    result = cc.getHash(large);
    a.checkEqual("getHash large", result.size(), 48U);
    a.checkEqualContent("getHash large content", ConstBytes_t(bytes), ConstBytes_t(large).trim(48));

    // Self-information
    a.checkEqual("getHashSize 1", cc.getHashSize(), 48U);
    a.checkEqual("getHashSize 2", cc.getHashSize(), cc.HASH_SIZE);
    a.checkEqual("getBlockSize", cc.getBlockSize(), 128U);
}

/** Hash tests from RFC 6234. */
AFL_TEST("afl.checksums.SHA384:getHash:rfc6234", a)
{
    static const TestCase cases[] = {
        { "abc", 1,
          0xCB, 0x00, 0x75, 0x3F, 0x45, 0xA3, 0x5E, 0x8B, 0xB5, 0xA0, 0x3D, 0x69, 0x9A, 0xC6, 0x50, 0x07, 0x27, 0x2C, 0x32, 0xAB, 0x0E, 0xDE, 0xD1, 0x63,
          0x1A, 0x8B, 0x60, 0x5A, 0x43, 0xFF, 0x5B, 0xED, 0x80, 0x86, 0x07, 0x2B, 0xA1, 0xE7, 0xCC, 0x23, 0x58, 0xBA, 0xEC, 0xA1, 0x34, 0xC8, 0x25, 0xA7 },
        { "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmn"
          "hijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu", 1,
          0x09, 0x33, 0x0C, 0x33, 0xF7, 0x11, 0x47, 0xE8, 0x3D, 0x19, 0x2F, 0xC7, 0x82, 0xCD, 0x1B, 0x47, 0x53, 0x11, 0x1B, 0x17, 0x3B, 0x3B, 0x05, 0xD2,
          0x2F, 0xA0, 0x80, 0x86, 0xE3, 0xB0, 0xF7, 0x12, 0xFC, 0xC7, 0xC7, 0x1A, 0x55, 0x7E, 0x2D, 0xB9, 0x66, 0xC3, 0xE9, 0xFA, 0x91, 0x74, 0x60, 0x39 },
        { "a", 1000000,
          0x9D, 0x0E, 0x18, 0x09, 0x71, 0x64, 0x74, 0xCB, 0x08, 0x6E, 0x83, 0x4E, 0x31, 0x0A, 0x4A, 0x1C, 0xED, 0x14, 0x9E, 0x9C, 0x00, 0xF2, 0x48, 0x52,
          0x79, 0x72, 0xCE, 0xC5, 0x70, 0x4C, 0x2A, 0x5B, 0x07, 0xB8, 0xB3, 0xDC, 0x38, 0xEC, 0xC4, 0xEB, 0xAE, 0x97, 0xDD, 0xD8, 0x7F, 0x3D, 0x89, 0x85 },
        { "0123456701234567012345670123456701234567012345670123456701234567", 10,
          0x2F, 0xC6, 0x4A, 0x4F, 0x50, 0x0D, 0xDB, 0x68, 0x28, 0xF6, 0xA3, 0x43, 0x0B, 0x8D, 0xD7, 0x2A, 0x36, 0x8E, 0xB7, 0xF3, 0xA8, 0x32, 0x2A, 0x70,
          0xBC, 0x84, 0x27, 0x5B, 0x9C, 0x0B, 0x3A, 0xB0, 0x0D, 0x27, 0xA5, 0xCC, 0x3C, 0x2D, 0x22, 0x4A, 0xA6, 0xB6, 0x1A, 0x0D, 0x79, 0xFB, 0x45, 0x96 },
        { "\xb9", 1,
          0xBC, 0x80, 0x89, 0xA1, 0x90, 0x07, 0xC0, 0xB1, 0x41, 0x95, 0xF4, 0xEC, 0xC7, 0x40, 0x94, 0xFE, 0xC6, 0x4F, 0x01, 0xF9, 0x09, 0x29, 0x28, 0x2C,
          0x2F, 0xB3, 0x92, 0x88, 0x15, 0x78, 0x20, 0x8A, 0xD4, 0x66, 0x82, 0x8B, 0x1C, 0x6C, 0x28, 0x3D, 0x27, 0x22, 0xCF, 0x0A, 0xD1, 0xAB, 0x69, 0x38 },
        { "\xa4\x1c\x49\x77\x79\xc0\x37\x5f\xf1\x0a\x7f\x4e\x08\x59\x17\x39", 1,
          0xC9, 0xA6, 0x84, 0x43, 0xA0, 0x05, 0x81, 0x22, 0x56, 0xB8, 0xEC, 0x76, 0xB0, 0x05, 0x16, 0xF0, 0xDB, 0xB7, 0x4F, 0xAB, 0x26, 0xD6, 0x65, 0x91,
          0x3F, 0x19, 0x4B, 0x6F, 0xFB, 0x0E, 0x91, 0xEA, 0x99, 0x67, 0x56, 0x6B, 0x58, 0x10, 0x9C, 0xBC, 0x67, 0x5C, 0xC2, 0x08, 0xE4, 0xC8, 0x23, 0xF7 },
        { "\x39\x96\x69\xe2\x8f\x6b\x9c\x6d\xbc\xbb\x69\x12\xec\x10\xff\xcf"
          "\x74\x79\x03\x49\xb7\xdc\x8f\xbe\x4a\x8e\x7b\x3b\x56\x21\xdb\x0f"
          "\x3e\x7d\xc8\x7f\x82\x32\x64\xbb\xe4\x0d\x18\x11\xc9\xea\x20\x61"
          "\xe1\xc8\x4a\xd1\x0a\x23\xfa\xc1\x72\x7e\x72\x02\xfc\x3f\x50\x42"
          "\xe6\xbf\x58\xcb\xa8\xa2\x74\x6e\x1f\x64\xf9\xb9\xea\x35\x2c\x71"
          "\x15\x07\x05\x3c\xf4\xe5\x33\x9d\x52\x86\x5f\x25\xcc\x22\xb5\xe8"
          "\x77\x84\xa1\x2f\xc9\x61\xd6\x6c\xb6\xe8\x95\x73\x19\x9a\x2c\xe6"
          "\x56\x5c\xbd\xf1\x3d\xca\x40\x38\x32\xcf\xcb\x0e\x8b\x72\x11\xe8"
          "\x3a\xf3\x2a\x11\xac\x17\x92\x9f\xf1\xc0\x73\xa5\x1c\xc0\x27\xaa"
          "\xed\xef\xf8\x5a\xad\x7c\x2b\x7c\x5a\x80\x3e\x24\x04\xd9\x6d\x2a"
          "\x77\x35\x7b\xda\x1a\x6d\xae\xed\x17\x15\x1c\xb9\xbc\x51\x25\xa4"
          "\x22\xe9\x41\xde\x0c\xa0\xfc\x50\x11\xc2\x3e\xcf\xfe\xfd\xd0\x96"
          "\x76\x71\x1c\xf3\xdb\x0a\x34\x40\x72\x0e\x16\x15\xc1\xf2\x2f\xbc"
          "\x3c\x72\x1d\xe5\x21\xe1\xb9\x9b\xa1\xbd\x55\x77\x40\x86\x42\x14"
          "\x7e\xd0\x96", 1,
          0x4F, 0x44, 0x0D, 0xB1, 0xE6, 0xED, 0xD2, 0x89, 0x9F, 0xA3, 0x35, 0xF0, 0x95, 0x15, 0xAA, 0x02, 0x5E, 0xE1, 0x77, 0xA7, 0x9F, 0x4B, 0x4A, 0xAF,
          0x38, 0xE4, 0x2B, 0x5C, 0x4D, 0xE6, 0x60, 0xF5, 0xDE, 0x8F, 0xB2, 0xA5, 0xB2, 0xFB, 0xD2, 0xA3, 0xCB, 0xFF, 0xD2, 0x0C, 0xFF, 0x12, 0x88, 0xC0 },
    };

    for (size_t i = 0; i < countof(cases); ++i) {
        // Compute the hash
        afl::checksums::SHA384 cc;
        for (size_t j = 0; j < cases[i].m_repeat; ++j) {
            cc.add(afl::string::toBytes(cases[i].m_string));
        }

        uint8_t buffer[64];
        a.check("", cc.getHash(buffer).equalContent(cases[i].m_expect));
    }
}

/** HMAC tests from RFC 6234. */
AFL_TEST("afl.checksums.SHA384:computeHMAC:rfc6234", a)
{
    afl::checksums::SHA384 cc;

    // 1
    cc.computeHMAC(afl::string::toBytes("\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b"),
                   afl::string::toBytes("\x48\x69\x20\x54\x68\x65\x72\x65"));
    a.checkEqual("case 1", cc.getHashAsHexString(), "afd03944d84895626b0825f4ab46907f15f9dadbe4101ec682aa034c7cebc59cfaea9ea9076ede7f4af152e8b2fa9cb6");

    // 2
    cc.computeHMAC(afl::string::toBytes("\x4a\x65\x66\x65"),
                   afl::string::toBytes("\x77\x68\x61\x74\x20\x64\x6f\x20\x79\x61\x20\x77\x61\x6e\x74"
                                        "\x20\x66\x6f\x72\x20\x6e\x6f\x74\x68\x69\x6e\x67\x3f"));
    a.checkEqual("case 2", cc.getHashAsHexString(), "af45d2e376484031617f78d2b58a6b1b9c7ef464f5a01b47e42ec3736322445e8e2240ca5e69e2c78b3239ecfab21649");

    // 3
    cc.computeHMAC(afl::string::toBytes("\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
                                        "\xaa\xaa\xaa\xaa\xaa"),
                   afl::string::toBytes("\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
                                        "\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
                                        "\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
                                        "\xdd\xdd\xdd\xdd\xdd"));
    a.checkEqual("case 3", cc.getHashAsHexString(), "88062608d3e6ad8a0aa2ace014c8a86f0aa635d947ac9febe83ef4e55966144b2a5ab39dc13814b94e3ab6e101a34f27");

    // 4
    cc.computeHMAC(afl::string::toBytes("\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
                                        "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19"),
                   afl::string::toBytes("\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd"
                                        "\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd"
                                        "\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd"
                                        "\xcd\xcd\xcd\xcd\xcd"));
    a.checkEqual("case 4", cc.getHashAsHexString(), "3e8a69b7783c25851933ab6290af6ca77a9981480850009cc5577c6e1f573b4e6801dd23c4a7d679ccf8a386c674cffb");
}
