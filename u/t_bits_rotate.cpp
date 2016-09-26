/**
  *  \file u/t_bits_rotate.cpp
  *  \brief Test for afl/bits/rotate.hpp
  */

#include "afl/bits/rotate.hpp"

#include "u/t_bits.hpp"

/** Test 8-bit rotate. */
void
TestBitsRotate::test8()
{
    for (int i = 0; i < 255; ++i) {
        TS_ASSERT_EQUALS(afl::bits::rotateLeft8(static_cast<uint8_t>(i), 0), i);
        TS_ASSERT_EQUALS(afl::bits::rotateLeft8(static_cast<uint8_t>(i), 8), i);
        TS_ASSERT_EQUALS(afl::bits::rotateLeft8(static_cast<uint8_t>(i), 800), i);
    }

    TS_ASSERT_EQUALS(afl::bits::rotateLeft8(0x03, 1), 0x06);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft8(0x03, 2), 0x0C);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft8(0x03, 3), 0x18);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft8(0x03, 4), 0x30);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft8(0x03, 5), 0x60);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft8(0x03, 6), 0xC0);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft8(0x03, 7), 0x81);

    TS_ASSERT_EQUALS(afl::bits::rotateLeft8(1, 1), 2);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft8(1, 2), 4);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft8(1, 4), 16);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft8(1, 8), 1);

    TS_ASSERT_EQUALS(afl::bits::rotateRight8(1, 1), 128);
    TS_ASSERT_EQUALS(afl::bits::rotateRight8(1, 2), 64);
    TS_ASSERT_EQUALS(afl::bits::rotateRight8(1, 4), 16);
    TS_ASSERT_EQUALS(afl::bits::rotateRight8(1, 8), 1);
}

/** Test 16-bit rotate. */
void
TestBitsRotate::test16()
{
    for (int i = 0; i < 255; ++i) {
        uint16_t i16 = static_cast<uint16_t>(i);
        TS_ASSERT_EQUALS(afl::bits::rotateLeft16(i16, 0), i);
        TS_ASSERT_EQUALS(afl::bits::rotateLeft16(i16, 8), i << 8); // no overflow yet!
        TS_ASSERT_EQUALS(afl::bits::rotateLeft16(i16, 16), i);
        TS_ASSERT_EQUALS(afl::bits::rotateLeft16(i16, 800), i);
    }

    TS_ASSERT_EQUALS(afl::bits::rotateLeft16(0x0003,  1), 0x0006);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft16(0x0003,  2), 0x000C);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft16(0x0003,  3), 0x0018);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft16(0x0003,  4), 0x0030);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft16(0x0003,  5), 0x0060);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft16(0x0003,  6), 0x00C0);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft16(0x0003,  7), 0x0180);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft16(0x0003,  8), 0x0300);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft16(0x0003,  9), 0x0600);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft16(0x0003, 10), 0x0C00);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft16(0x0003, 11), 0x1800);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft16(0x0003, 12), 0x3000);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft16(0x0003, 13), 0x6000);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft16(0x0003, 14), 0xC000);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft16(0x0003, 15), 0x8001);

    TS_ASSERT_EQUALS(afl::bits::rotateLeft16(1, 1), 2);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft16(1, 2), 4);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft16(1, 4), 16);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft16(1, 15), 32768U);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft16(1, 16), 1);

    TS_ASSERT_EQUALS(afl::bits::rotateRight16(1, 1), 32768U);
    TS_ASSERT_EQUALS(afl::bits::rotateRight16(1, 2), 16384U);
    TS_ASSERT_EQUALS(afl::bits::rotateRight16(1, 4), 4096U);
    TS_ASSERT_EQUALS(afl::bits::rotateRight16(1, 15), 2);
    TS_ASSERT_EQUALS(afl::bits::rotateRight16(1, 16), 1);
}

/** Test 32-bit rotate. */
void
TestBitsRotate::test32()
{
    for (uint32_t i = 0; i < 255; ++i) {
        TS_ASSERT_EQUALS(afl::bits::rotateLeft32(i, 0), i);
        TS_ASSERT_EQUALS(afl::bits::rotateLeft32(i, 8), i << 8);    // no overflow yet!
        TS_ASSERT_EQUALS(afl::bits::rotateLeft32(i, 16), i << 16);  // no overflow yet!
        TS_ASSERT_EQUALS(afl::bits::rotateLeft32(i, 24), i << 24);  // no overflow yet!
        TS_ASSERT_EQUALS(afl::bits::rotateLeft32(i, 32), i);
        TS_ASSERT_EQUALS(afl::bits::rotateLeft32(i, 800), i);
    }

    TS_ASSERT_EQUALS(afl::bits::rotateLeft32(0x00000003,  1), 0x00000006U);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft32(0x00000003,  2), 0x0000000CU);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft32(0x00000003,  3), 0x00000018U);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft32(0x00000003,  4), 0x00000030U);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft32(0x00000003,  5), 0x00000060U);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft32(0x00000003,  6), 0x000000C0U);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft32(0x00000003,  7), 0x00000180U);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft32(0x00000003,  8), 0x00000300U);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft32(0x00000003,  9), 0x00000600U);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft32(0x00000003, 10), 0x00000C00U);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft32(0x00000003, 11), 0x00001800U);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft32(0x00000003, 12), 0x00003000U);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft32(0x00000003, 13), 0x00006000U);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft32(0x00000003, 14), 0x0000C000U);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft32(0x00000003, 15), 0x00018000U);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft32(0x00000003, 16), 0x00030000U);

    TS_ASSERT_EQUALS(afl::bits::rotateLeft32(0x00000003, 30), 0xC0000000U);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft32(0x00000003, 31), 0x80000001U);

    TS_ASSERT_EQUALS(afl::bits::rotateLeft32(1, 1), 2U);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft32(1, 16), 65536U);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft32(0x12345678, 4), 0x23456781U);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft32(0xFFFFFFFF, 7), 0xFFFFFFFFU);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft32(1, 32), 1U);
    
    TS_ASSERT_EQUALS(afl::bits::rotateRight32(1, 1), 0x80000000U);
    TS_ASSERT_EQUALS(afl::bits::rotateRight32(1, 16), 65536U);
    TS_ASSERT_EQUALS(afl::bits::rotateRight32(0x12345678, 4), 0x81234567U);
    TS_ASSERT_EQUALS(afl::bits::rotateRight32(0xFFFFFFFF, 7), 0xFFFFFFFFU);
    TS_ASSERT_EQUALS(afl::bits::rotateRight32(1, 32), 1U);
}

/** Test 64-bit rotate. */
void
TestBitsRotate::test64()
{
    for (uint64_t i = 0; i < 255; ++i) {
        TS_ASSERT_EQUALS(afl::bits::rotateLeft64(i, 0), i);
        TS_ASSERT_EQUALS(afl::bits::rotateLeft64(i, 8), i << 8);    // no overflow yet!
        TS_ASSERT_EQUALS(afl::bits::rotateLeft64(i, 16), i << 16);  // no overflow yet!
        TS_ASSERT_EQUALS(afl::bits::rotateLeft64(i, 24), i << 24);  // no overflow yet!
        TS_ASSERT_EQUALS(afl::bits::rotateLeft64(i, 32), i << 32);
        TS_ASSERT_EQUALS(afl::bits::rotateLeft64(i, 800), i << 32); // wraps around 12 times, then shift by 16
    }

    TS_ASSERT_EQUALS(afl::bits::rotateLeft64(0x00000003,  1), 0x00000006ULL);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft64(0x00000003,  2), 0x0000000CULL);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft64(0x00000003,  3), 0x00000018ULL);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft64(0x00000003,  4), 0x00000030ULL);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft64(0x00000003,  5), 0x00000060ULL);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft64(0x00000003,  6), 0x000000C0ULL);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft64(0x00000003,  7), 0x00000180ULL);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft64(0x00000003,  8), 0x00000300ULL);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft64(0x00000003,  9), 0x00000600ULL);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft64(0x00000003, 10), 0x00000C00ULL);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft64(0x00000003, 11), 0x00001800ULL);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft64(0x00000003, 12), 0x00003000ULL);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft64(0x00000003, 13), 0x00006000ULL);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft64(0x00000003, 14), 0x0000C000ULL);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft64(0x00000003, 15), 0x00018000ULL);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft64(0x00000003, 16), 0x00030000ULL);

    TS_ASSERT_EQUALS(afl::bits::rotateLeft64(0x00000003, 30), 0x0C0000000ULL);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft64(0x00000003, 31), 0x180000000ULL);

    TS_ASSERT_EQUALS(afl::bits::rotateLeft64(0x00000003, 62), 0xC000000000000000ULL);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft64(0x00000003, 63), 0x8000000000000001ULL);

    TS_ASSERT_EQUALS(afl::bits::rotateLeft64(1, 1), 2ULL);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft64(1, 16), 65536ULL);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft64(0x123456789ABCDEF0ULL, 4), 0x23456789ABCDEF01ULL);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft64(0xFFFFFFFFFFFFFFFFULL, 7), 0xFFFFFFFFFFFFFFFFULL);
    TS_ASSERT_EQUALS(afl::bits::rotateLeft64(1, 64), 1ULL);
    
    TS_ASSERT_EQUALS(afl::bits::rotateRight64(1, 1), 0x8000000000000000ULL);
    TS_ASSERT_EQUALS(afl::bits::rotateRight64(1, 48), 65536ULL);
    TS_ASSERT_EQUALS(afl::bits::rotateRight64(0x123456789ABCDEF0ULL, 4), 0x0123456789ABCDEFULL);
    TS_ASSERT_EQUALS(afl::bits::rotateRight64(0xFFFFFFFFFFFFFFFFULL, 7), 0xFFFFFFFFFFFFFFFFULL);
    TS_ASSERT_EQUALS(afl::bits::rotateRight64(1, 64), 1ULL);
}
