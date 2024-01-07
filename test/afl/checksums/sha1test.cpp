/**
  *  \file test/afl/checksums/sha1test.cpp
  *  \brief Test for afl::checksums::SHA1
  */

#include "afl/checksums/sha1.hpp"
#include "afl/test/testrunner.hpp"
#include "afl/string/string.hpp"
#include "afl/base/countof.hpp"

using afl::base::ConstBytes_t;

namespace {
    struct TestCase {
        const char* m_string;
        uint32_t m_repeat;
        const uint8_t m_expect[20];
    };
}

AFL_TEST("afl.checksums.SHA1", a)
{
    afl::checksums::SHA1 cc;
    uint8_t bytes[20];

    // Querying an empty hash should return the expected result
    static const uint8_t expected[] = { 0xda, 0x39, 0xa3, 0xee, 0x5e, 0x6b, 0x4b, 0x0d, 0x32, 0x55,
                                        0xbf, 0xef, 0x95, 0x60, 0x18, 0x90, 0xaf, 0xd8, 0x07, 0x09 };
    ConstBytes_t result = cc.getHash(bytes);
    a.check("getHash empty", result.equalContent(expected));

    // Asking again should get the same result again
    result = cc.getHash(bytes);
    a.check("getHash empty 2", result.equalContent(expected));

    // Check string
    a.checkEqual("getHashAsHexString", cc.getHashAsHexString(), "da39a3ee5e6b4b0d3255bfef95601890afd80709");

    // Asking a smaller buffer, must get correct content (first half of hash)
    uint8_t small[8];
    result = cc.getHash(small);
    a.checkEqual("getHash small", result.size(), 8U);
    a.checkEqualContent("getHash small content", ConstBytes_t(bytes).trim(8), ConstBytes_t(small));

    // Asking a larger buffer, must get correct content (just the hash)
    uint8_t large[64];
    result = cc.getHash(large);
    a.checkEqual("getHash large", result.size(), 20U);
    a.checkEqualContent("getHash large content", ConstBytes_t(bytes), ConstBytes_t(large).trim(20));

    // Self-information
    a.checkEqual("getHashSize 1", cc.getHashSize(), 20U);
    a.checkEqual("getHashSize 2", cc.getHashSize(), cc.HASH_SIZE);
}

/** Test testsuite from RFC 3174. */
AFL_TEST("afl.checksums.SHA1:rfc3174", a)
{
    static const TestCase cases[] = {
        { "abc", 1,
          { 0xA9, 0x99, 0x3E, 0x36, 0x47, 0x06, 0x81, 0x6A, 0xBA, 0x3E,
            0x25, 0x71, 0x78, 0x50, 0xC2, 0x6C, 0x9C, 0xD0, 0xD8, 0x9D } },
        { "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 1,
          { 0x84, 0x98, 0x3E, 0x44, 0x1C, 0x3B, 0xD2, 0x6E, 0xBA, 0xAE,
            0x4A, 0xA1, 0xF9, 0x51, 0x29, 0xE5, 0xE5, 0x46, 0x70, 0xF1 } },
        { "a", 1000000,
          { 0x34, 0xAA, 0x97, 0x3C, 0xD4, 0xC4, 0xDA, 0xA4, 0xF6, 0x1E,
            0xEB, 0x2B, 0xDB, 0xAD, 0x27, 0x31, 0x65, 0x34, 0x01, 0x6F } },
        { "0123456701234567012345670123456701234567012345670123456701234567",
          10,
          { 0xDE, 0xA3, 0x56, 0xA2, 0xCD, 0xDD, 0x90, 0xC7, 0xA7, 0xEC,
            0xED, 0xC5, 0xEB, 0xB5, 0x63, 0x93, 0x4F, 0x46, 0x04, 0x52 } },
    };

    for (size_t i = 0; i < countof(cases); ++i) {
        // Compute the hash
        afl::checksums::SHA1 cc;
        for (size_t j = 0; j < cases[i].m_repeat; ++j) {
            cc.add(afl::string::toBytes(cases[i].m_string));
        }

        uint8_t buffer[64];
        a.check("", cc.getHash(buffer).equalContent(cases[i].m_expect));
    }
}

/** Test speed. */
AFL_TEST("afl.checksums.SHA1:speed", a)
{
    afl::checksums::SHA1 cc;
    static const uint8_t zeropage[4096] = {};

    // 100 MiB
    for (size_t i = 0; i < 256*100; ++i) {
        cc.add(zeropage);
    }

    static const uint8_t expect[] = { 0x2c, 0x2c, 0xec, 0xcb, 0x5e, 0xc5, 0x57, 0x4f, 0x79, 0x1d,
                                      0x45, 0xb6, 0x3c, 0x94, 0x0c, 0xff, 0x20, 0x55, 0x0f, 0x9a };
    uint8_t bytes[20];
    a.check("", cc.getHash(bytes).equalContent(expect));
}

/** Test computePBKDF2 using examples from RFC 6070. */
AFL_TEST("afl.checksums.SHA1:computePBKDF2:rfc6070", a)
{
    afl::checksums::SHA1 cc;

    {
        uint8_t out[20];        // dkLen = 20
        cc.computePBKDF2(out,
                         afl::string::toBytes("password"),
                         afl::string::toBytes("salt"),
                         1);
        static const uint8_t expect[20] = {
            0x0C, 0x60, 0xC8, 0x0F, 0x96, 0x1F, 0x0E, 0x71,
            0xF3, 0xA9, 0xB5, 0x24, 0xAF, 0x60, 0x12, 0x06,
            0x2F, 0xE0, 0x37, 0xA6
        };
        a.checkEqualContent("case 1", ConstBytes_t(out), ConstBytes_t(expect));
    }

    {
        uint8_t out[20];        // dkLen = 20
        cc.computePBKDF2(out,
                         afl::string::toBytes("password"),
                         afl::string::toBytes("salt"),
                         2);
        static const uint8_t expect[20] = {
            0xEA, 0x6C, 0x01, 0x4D, 0xC7, 0x2D, 0x6F, 0x8C,
            0xCD, 0x1E, 0xD9, 0x2A, 0xCE, 0x1D, 0x41, 0xF0,
            0xD8, 0xDE, 0x89, 0x57
        };
        a.checkEqualContent("case 2", ConstBytes_t(out), ConstBytes_t(expect));
    }

    {
        uint8_t out[20];        // dkLen = 20
        cc.computePBKDF2(out,
                         afl::string::toBytes("password"),
                         afl::string::toBytes("salt"),
                         4096);
        static const uint8_t expect[20] = {
            0x4b, 0x00, 0x79, 0x01, 0xb7, 0x65, 0x48, 0x9a,
            0xbe, 0xad, 0x49, 0xd9, 0x26, 0xf7, 0x21, 0xd0,
            0x65, 0xa4, 0x29, 0xc1
        };
        a.checkEqualContent("case 3", ConstBytes_t(out), ConstBytes_t(expect));
    }

    // Disabled because it runs very long:
    // {
    //     uint8_t out[20];        // dkLen = 20
    //     cc.computePBKDF2(out,
    //                      afl::string::toBytes("password"),
    //                      afl::string::toBytes("salt"),
    //                      16777216);
    //     static const uint8_t expect[20] = {
    //         0xee, 0xfe, 0x3d, 0x61, 0xcd, 0x4d, 0xa4, 0xe4,
    //         0xe9, 0x94, 0x5b, 0x3d, 0x6b, 0xa2, 0x15, 0x8c,
    //         0x26, 0x34, 0xe9, 0x84
    //     };
    //     a.checkEqualContent("case 4", ConstBytes_t(out), ConstBytes_t(expect));
    // }

    {
        uint8_t out[25];        // dkLen = 25
        cc.computePBKDF2(out,
                         afl::string::toBytes("passwordPASSWORDpassword"),
                         afl::string::toBytes("saltSALTsaltSALTsaltSALTsaltSALTsalt"),
                         4096);
        static const uint8_t expect[25] = {
            0x3d, 0x2e, 0xec, 0x4f, 0xe4, 0x1c, 0x84, 0x9b,
            0x80, 0xc8, 0xd8, 0x36, 0x62, 0xc0, 0xe4, 0x4a,
            0x8b, 0x29, 0x1a, 0x96, 0x4c, 0xf2, 0xf0, 0x70,
            0x38,
        };
        a.checkEqualContent("case 5", ConstBytes_t(out), ConstBytes_t(expect));
    }

    {
        uint8_t out[16];        // dkLen = 16
        static const uint8_t password[]  = {'p','a','s','s','\0','w','o','r','d'};
        static const uint8_t salt[]      = {'s','a','\0','l','t'};
        cc.computePBKDF2(out, password, salt, 4096);
        static const uint8_t expect[16] = {
            0x56, 0xfa, 0x6a, 0xa7, 0x55, 0x48, 0x09, 0x9d,
            0xcc, 0x37, 0xd7, 0xf0, 0x34, 0x25, 0xe0, 0xc3
        };
        a.checkEqualContent("case 6", ConstBytes_t(out), ConstBytes_t(expect));
    }
}

/** Hash tests from RFC 6234. */
AFL_TEST("afl.checksums.SHA1:rfc6232", a)
{
    static const TestCase cases[] = {
        { "abc", 1,
          { 0xA9, 0x99, 0x3E, 0x36, 0x47, 0x06, 0x81, 0x6A, 0xBA, 0x3E, 0x25, 0x71, 0x78, 0x50, 0xC2, 0x6C, 0x9C, 0xD0, 0xD8, 0x9D } },
        { "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 1,
          { 0x84, 0x98, 0x3E, 0x44, 0x1C, 0x3B, 0xD2, 0x6E, 0xBA, 0xAE, 0x4A, 0xA1, 0xF9, 0x51, 0x29, 0xE5, 0xE5, 0x46, 0x70, 0xF1 } },
        { "a", 1000000,
          { 0x34, 0xAA, 0x97, 0x3C, 0xD4, 0xC4, 0xDA, 0xA4, 0xF6, 0x1E, 0xEB, 0x2B, 0xDB, 0xAD, 0x27, 0x31, 0x65, 0x34, 0x01, 0x6F } },
        { "0123456701234567012345670123456701234567012345670123456701234567", 10,
          { 0xDE, 0xA3, 0x56, 0xA2, 0xCD, 0xDD, 0x90, 0xC7, 0xA7, 0xEC, 0xED, 0xC5, 0xEB, 0xB5, 0x63, 0x93, 0x4F, 0x46, 0x04, 0x52 } },
        { "\x5e", 1,
          { 0x5E, 0x6F, 0x80, 0xA3, 0x4A, 0x97, 0x98, 0xCA, 0xFC, 0x6A, 0x5D, 0xB9, 0x6C, 0xC5, 0x7B, 0xA4, 0xC4, 0xDB, 0x59, 0xC2 } },
        { "\x9a\x7d\xfd\xf1\xec\xea\xd0\x6e\xd6\x46\xaa\x55\xfe\x75\x71\x46", 1,
          { 0x82, 0xAB, 0xFF, 0x66, 0x05, 0xDB, 0xE1, 0xC1, 0x7D, 0xEF, 0x12, 0xA3, 0x94, 0xFA, 0x22, 0xA8, 0x2B, 0x54, 0x4A, 0x35 } },
    };


    for (size_t i = 0; i < countof(cases); ++i) {
        // Compute the hash
        afl::checksums::SHA1 cc;
        for (size_t j = 0; j < cases[i].m_repeat; ++j) {
            cc.add(afl::string::toBytes(cases[i].m_string));
        }

        uint8_t buffer[64];
        a.check("", cc.getHash(buffer).equalContent(cases[i].m_expect));
    }
}

/** HMAC tests from RFC 6234. */
AFL_TEST("afl.checksums.SHA1:computeHMAC:rfc6234", a)
{
    afl::checksums::SHA1 cc;

    // 1
    cc.computeHMAC(afl::string::toBytes("\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b"),
                   afl::string::toBytes("\x48\x69\x20\x54\x68\x65\x72\x65"));
    a.checkEqual("case 1", cc.getHashAsHexString(), "b617318655057264e28bc0b6fb378c8ef146be00");

    // 2
    cc.computeHMAC(afl::string::toBytes("\x4a\x65\x66\x65"),
                   afl::string::toBytes("\x77\x68\x61\x74\x20\x64\x6f\x20\x79\x61\x20\x77\x61\x6e\x74"
                                        "\x20\x66\x6f\x72\x20\x6e\x6f\x74\x68\x69\x6e\x67\x3f"));
    a.checkEqual("case 2", cc.getHashAsHexString(), "effcdf6ae5eb2fa2d27416d5f184df9c259a7c79");

    // 3
    cc.computeHMAC(afl::string::toBytes("\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
                                        "\xaa\xaa\xaa\xaa\xaa"),
                   afl::string::toBytes("\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
                                        "\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
                                        "\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
                                        "\xdd\xdd\xdd\xdd\xdd"));
    a.checkEqual("case 3", cc.getHashAsHexString(), "125d7342b9ac11cd91a39af48aa17b4f63f175d3");

    // 4
    cc.computeHMAC(afl::string::toBytes("\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
                                        "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19"),
                   afl::string::toBytes("\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd"
                                        "\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd"
                                        "\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd"
                                        "\xcd\xcd\xcd\xcd\xcd"));
    a.checkEqual("case 4", cc.getHashAsHexString(), "4c9007f4026250c6bc8414f9bf50c86c2d7235da");
}
