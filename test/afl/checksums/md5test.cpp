/**
  *  \file test/afl/checksums/md5test.cpp
  *  \brief Test for afl::checksums::MD5
  */

#include "afl/checksums/md5.hpp"
#include "afl/test/testrunner.hpp"

#include "afl/base/countof.hpp"
#include "afl/string/string.hpp"

using afl::base::ConstBytes_t;

namespace {
    struct Tests {
        const char* m_data;
        const uint8_t m_expected[16];
    };
}

/** Basic tests. */
AFL_TEST("afl.checksums.MD5", a)
{
    afl::checksums::MD5 md5;
    uint8_t bytes[16];

    // Querying an empty hash should return the expected result
    static const uint8_t expected[] = { 0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04, 0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e };
    ConstBytes_t result = md5.getHash(bytes);
    a.check("getHash empty", result.equalContent(expected));

    // Asking again should get the same result again
    result = md5.getHash(bytes);
    a.check("getHash empty 2", result.equalContent(expected));

    // Check string
    a.checkEqual("getHashAsHexString", md5.getHashAsHexString(), "d41d8cd98f00b204e9800998ecf8427e");

    // Asking a smaller buffer, must get correct content (first half of hash)
    uint8_t small[8];
    result = md5.getHash(small);
    a.checkEqual("getHash small", result.size(), 8U);
    a.checkEqualContent("getHash small content", ConstBytes_t(bytes).trim(8), ConstBytes_t(small));

    // Asking a larger buffer, must get correct content (just the hash)
    uint8_t large[64];
    result = md5.getHash(large);
    a.checkEqual("getHash large", result.size(), 16U);
    a.checkEqualContent("getHash large content", ConstBytes_t(bytes), ConstBytes_t(large).trim(16));

    // Self-information
    a.checkEqual("getHashSize 1", md5.getHashSize(), 16U);
    a.checkEqual("getHashSize 2", md5.getHashSize(), md5.HASH_SIZE);
}

/** Test testsuite from RFC 1321. */
AFL_TEST("afl.checksums.MD5:rfc1321", a)
{
    // Definition of the test cases
    static const Tests tests[] = {
        { "",
          { 0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04, 0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e } },
        { "a",
          { 0x0c, 0xc1, 0x75, 0xb9, 0xc0, 0xf1, 0xb6, 0xa8, 0x31, 0xc3, 0x99, 0xe2, 0x69, 0x77, 0x26, 0x61 } },
        { "abc",
          { 0x90, 0x01, 0x50, 0x98, 0x3c, 0xd2, 0x4f, 0xb0, 0xd6, 0x96, 0x3f, 0x7d, 0x28, 0xe1, 0x7f, 0x72 } },
        { "message digest",
          { 0xf9, 0x6b, 0x69, 0x7d, 0x7c, 0xb7, 0x93, 0x8d, 0x52, 0x5a, 0x2f, 0x31, 0xaa, 0xf1, 0x61, 0xd0 } },
        { "abcdefghijklmnopqrstuvwxyz",
          { 0xc3, 0xfc, 0xd3, 0xd7, 0x61, 0x92, 0xe4, 0x00, 0x7d, 0xfb, 0x49, 0x6c, 0xca, 0x67, 0xe1, 0x3b } },
        { "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
          { 0xd1, 0x74, 0xab, 0x98, 0xd2, 0x77, 0xd9, 0xf5, 0xa5, 0x61, 0x1c, 0x2c, 0x9f, 0x41, 0x9d, 0x9f } },
        { "12345678901234567890123456789012345678901234567890123456789012345678901234567890",
          { 0x57, 0xed, 0xf4, 0xa2, 0x2b, 0xe3, 0xc9, 0x55, 0xac, 0x49, 0xda, 0x2e, 0x21, 0x07, 0xb6, 0x7a } },
    };

    for (size_t i = 0; i < countof(tests); ++i) {
        // Compute checksum
        afl::checksums::MD5 md5;
        md5.add(afl::string::toBytes(tests[i].m_data));

        // Check result
        uint8_t bytes[16];
        ConstBytes_t result = md5.getHash(bytes);
        a.check("", result.equalContent(tests[i].m_expected));
    }
}

/** Test that we can split a hash operation and still get correct intermediate results. */
AFL_TEST("afl.checksums.MD5:split", a)
{
    // First part of message
    afl::checksums::MD5 md5;
    md5.add(afl::string::toBytes("abc"));
    static const uint8_t expect1[] = { 0x90, 0x01, 0x50, 0x98, 0x3c, 0xd2, 0x4f, 0xb0, 0xd6, 0x96, 0x3f, 0x7d, 0x28, 0xe1, 0x7f, 0x72 };

    uint8_t bytes[16];
    a.check("getHash 1", md5.getHash(bytes).equalContent(expect1));

    // Second part of message
    md5.add(afl::string::toBytes("defghijklmnopqrstuvwxyz"));
    static const uint8_t expect2[] = { 0xc3, 0xfc, 0xd3, 0xd7, 0x61, 0x92, 0xe4, 0x00, 0x7d, 0xfb, 0x49, 0x6c, 0xca, 0x67, 0xe1, 0x3b };
    a.check("getHash 2", md5.getHash(bytes).equalContent(expect2));

    // Clear; must report correct result for empty hash
    md5.clear();
    static const uint8_t expect3[] = { 0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04, 0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e };
    a.check("getHash 3", md5.getHash(bytes).equalContent(expect3));
}

/** Test overall speed. */
AFL_TEST("afl.checksums.MD5:speed", a)
{
    afl::checksums::MD5 md5;
    static const uint8_t zeropage[4096] = {};

    // 100 MiB
    for (size_t i = 0; i < 256*100; ++i) {
        md5.add(zeropage);
    }

    static const uint8_t expect[] = { 0x2f, 0x28, 0x2b, 0x84, 0xe7, 0xe6, 0x08, 0xd5, 0x85, 0x24, 0x49, 0xed, 0x94, 0x0b, 0xfc, 0x51 };
    uint8_t bytes[16];
    a.check("", md5.getHash(bytes).equalContent(expect));
}

/** Test test vectors from RFC 2104. */
AFL_TEST("afl.checksums.MD5:computeHMAC:rfc2104", a)
{
    afl::checksums::MD5 md5;
    uint8_t bytes[16];

    // First test vector
    static const uint8_t key1[] = { 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b };
    static const uint8_t expect1[] = { 0x92, 0x94, 0x72, 0x7a, 0x36, 0x38, 0xbb, 0x1c, 0x13, 0xf4, 0x8e, 0xf8, 0x15, 0x8b, 0xfc, 0x9d };
    md5.computeHMAC(key1, afl::string::toBytes("Hi There"));
    a.check("getHash 1", md5.getHash(bytes).equalContent(expect1));

    // Second test vector
    static const uint8_t expect2[] = { 0x75, 0x0c, 0x78, 0x3e, 0x6a, 0xb0, 0xb5, 0x03, 0xea, 0xa8, 0x6e, 0x31, 0x0a, 0x5d, 0xb7, 0x38 };
    md5.computeHMAC(afl::string::toBytes("Jefe"),
                    afl::string::toBytes("what do ya want for nothing?"));
    a.check("getHash 2", md5.getHash(bytes).equalContent(expect2));

    // Third test vector
    static const uint8_t key3[] = { 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA };
    static const uint8_t data3[50] = { 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
                                       0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
                                       0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
                                       0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
                                       0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD };
    static const uint8_t expect3[] = { 0x56, 0xbe, 0x34, 0x52, 0x1d, 0x14, 0x4c, 0x88, 0xdb, 0xb8, 0xc7, 0x33, 0xf0, 0xe8, 0xb3, 0xf6 };
    md5.computeHMAC(key3, data3);
    a.check("getHash 3", md5.getHash(bytes).equalContent(expect3));
}
