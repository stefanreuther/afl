/**
  *  \file u/t_checksums_sha1.cpp
  *  \brief Test for afl::checksums::SHA1
  */

#include "afl/checksums/sha1.hpp"

#include "u/t_checksums.hpp"
#include "afl/string/string.hpp"
#include "afl/base/countof.hpp"

namespace {
    struct TestCase {
        const char* m_string;
        uint32_t m_repeat;
        const uint8_t m_expect[20];
    };
}

/** Basic tests. */
void
TestChecksumsSHA1::testBasic()
{
    afl::checksums::SHA1 cc;
    uint8_t bytes[20];

    // Querying an empty hash should return the expected result
    static const uint8_t expected[] = { 0xda, 0x39, 0xa3, 0xee, 0x5e, 0x6b, 0x4b, 0x0d, 0x32, 0x55,
                                        0xbf, 0xef, 0x95, 0x60, 0x18, 0x90, 0xaf, 0xd8, 0x07, 0x09 };
    afl::base::ConstBytes_t result = cc.getHash(bytes);
    TS_ASSERT(result.equalContent(expected));

    // Asking again should get the same result again
    result = cc.getHash(bytes);
    TS_ASSERT(result.equalContent(expected));

    // Check string
    TS_ASSERT_EQUALS(cc.getHashAsHexString(), "da39a3ee5e6b4b0d3255bfef95601890afd80709");

    // Asking a smaller buffer, must get correct content (first half of hash)
    uint8_t small[8];
    result = cc.getHash(small);
    TS_ASSERT_EQUALS(result.size(), 8U);
    TS_ASSERT_SAME_DATA(bytes, small, 8);

    // Asking a larger buffer, must get correct content (just the hash)
    uint8_t large[64];
    result = cc.getHash(large);
    TS_ASSERT_EQUALS(result.size(), 20U);
    TS_ASSERT_SAME_DATA(bytes, large, 20);

    // Self-information
    TS_ASSERT_EQUALS(cc.getHashSize(), 20U);
}

/** Test testsuite from RFC 3174. */
void
TestChecksumsSHA1::testSuite()
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
        TS_ASSERT(cc.getHash(buffer).equalContent(cases[i].m_expect));
    }
}

/** Test speed. */
void
TestChecksumsSHA1::testSpeed()
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
    TS_ASSERT(cc.getHash(bytes).equalContent(expect));
}

/** Test computePBKDF2 using examples from RFC 6070. */
void
TestChecksumsSHA1::testPBKDF2()
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
        TS_ASSERT_SAME_DATA(out, expect, 20);
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
        TS_ASSERT_SAME_DATA(out, expect, 20);
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
        TS_ASSERT_SAME_DATA(out, expect, 20);
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
    //     TS_ASSERT_SAME_DATA(out, expect, 20);
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
        TS_ASSERT_SAME_DATA(out, expect, 25);
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
        TS_ASSERT_SAME_DATA(out, expect, 16);
    }
}
