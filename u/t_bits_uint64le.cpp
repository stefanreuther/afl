/**
  *  \file u/t_bits_uint64le.cpp
  *  \brief Test for afl::bits::UInt64LE
  */

#include "afl/bits/uint64le.hpp"
#include "u/t_bits.hpp"

/** Simple tests. */
void
TestBitsUInt64LE::testIt()
{
    // Unpacking regular + border cases
    uint8_t a1[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    TS_ASSERT_EQUALS(afl::bits::UInt64LE::unpack(a1), 0x0807060504030201ULL);

    uint8_t a2[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    TS_ASSERT_EQUALS(afl::bits::UInt64LE::unpack(a2), 0x0000000000000000ULL);

    uint8_t a3[8] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    TS_ASSERT_EQUALS(afl::bits::UInt64LE::unpack(a3), 0xFFFFFFFFFFFFFFFFULL);

    uint8_t a4[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80 };
    TS_ASSERT_EQUALS(afl::bits::UInt64LE::unpack(a4), 0x8000000000000000ULL);

    uint8_t a5[8] = { 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    TS_ASSERT_EQUALS(afl::bits::UInt64LE::unpack(a5), 0x00000000000000FFULL);

    uint8_t a6[8] = { 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00 };
    TS_ASSERT_EQUALS(afl::bits::UInt64LE::unpack(a6), 0x000000FF00000000ULL);

    // Packing
    uint8_t a[8];
    afl::bits::UInt64LE::pack(a, 0);
    TS_ASSERT_EQUALS(a[0], 0);
    TS_ASSERT_EQUALS(a[1], 0);
    TS_ASSERT_EQUALS(a[2], 0);
    TS_ASSERT_EQUALS(a[3], 0);
    TS_ASSERT_EQUALS(a[4], 0);
    TS_ASSERT_EQUALS(a[5], 0);
    TS_ASSERT_EQUALS(a[6], 0);
    TS_ASSERT_EQUALS(a[7], 0);

    afl::bits::UInt64LE::pack(a, 0xFFFFFFFFFFFFFFFFULL);
    TS_ASSERT_EQUALS(a[0], 0xFF);
    TS_ASSERT_EQUALS(a[1], 0xFF);
    TS_ASSERT_EQUALS(a[2], 0xFF);
    TS_ASSERT_EQUALS(a[3], 0xFF);
    TS_ASSERT_EQUALS(a[4], 0xFF);
    TS_ASSERT_EQUALS(a[5], 0xFF);
    TS_ASSERT_EQUALS(a[6], 0xFF);
    TS_ASSERT_EQUALS(a[7], 0xFF);

    afl::bits::UInt64LE::pack(a, 0x8000000000000000ULL);
    TS_ASSERT_EQUALS(a[0], 0x00);
    TS_ASSERT_EQUALS(a[1], 0x00);
    TS_ASSERT_EQUALS(a[2], 0x00);
    TS_ASSERT_EQUALS(a[3], 0x00);
    TS_ASSERT_EQUALS(a[4], 0x00);
    TS_ASSERT_EQUALS(a[5], 0x00);
    TS_ASSERT_EQUALS(a[6], 0x00);
    TS_ASSERT_EQUALS(a[7], 0x80);

    afl::bits::UInt64LE::pack(a, 0x123456789ABCDEF0ULL);
    TS_ASSERT_EQUALS(a[0], 0xF0);
    TS_ASSERT_EQUALS(a[1], 0xDE);
    TS_ASSERT_EQUALS(a[2], 0xBC);
    TS_ASSERT_EQUALS(a[3], 0x9A);
    TS_ASSERT_EQUALS(a[4], 0x78);
    TS_ASSERT_EQUALS(a[5], 0x56);
    TS_ASSERT_EQUALS(a[6], 0x34);
    TS_ASSERT_EQUALS(a[7], 0x12);

    afl::bits::UInt64LE::pack(a, 0x00000000000000FFULL);
    TS_ASSERT_EQUALS(a[0], 0xFF);
    TS_ASSERT_EQUALS(a[1], 0x00);
    TS_ASSERT_EQUALS(a[2], 0x00);
    TS_ASSERT_EQUALS(a[3], 0x00);
    TS_ASSERT_EQUALS(a[4], 0x00);
    TS_ASSERT_EQUALS(a[5], 0x00);
    TS_ASSERT_EQUALS(a[6], 0x00);
    TS_ASSERT_EQUALS(a[7], 0x00);
}
