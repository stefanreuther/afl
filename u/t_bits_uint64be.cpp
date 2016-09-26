/**
  *  \file u/t_bits_uint64be.cpp
  *  \brief Test for afl::bits::UInt64BE
  */

#include "afl/bits/uint64be.hpp"
#include "u/t_bits.hpp"

/** Simple tests. */
void
TestBitsUInt64BE::testIt()
{
    // Unpacking regular + border cases
    uint8_t a1[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    TS_ASSERT_EQUALS(afl::bits::UInt64BE::unpack(a1), 0x0102030405060708ULL);

    uint8_t a2[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    TS_ASSERT_EQUALS(afl::bits::UInt64BE::unpack(a2), 0x0000000000000000ULL);

    uint8_t a3[8] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    TS_ASSERT_EQUALS(afl::bits::UInt64BE::unpack(a3), 0xFFFFFFFFFFFFFFFFULL);

    uint8_t a4[8] = { 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    TS_ASSERT_EQUALS(afl::bits::UInt64BE::unpack(a4), 0x8000000000000000ULL);

    uint8_t a5[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF };
    TS_ASSERT_EQUALS(afl::bits::UInt64BE::unpack(a5), 0x00000000000000FFULL);

    uint8_t a6[8] = { 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00 };
    TS_ASSERT_EQUALS(afl::bits::UInt64BE::unpack(a6), 0x000000FF00000000ULL);

    // Packing
    uint8_t a[8];
    afl::bits::UInt64BE::pack(a, 0);
    TS_ASSERT_EQUALS(a[0], 0);
    TS_ASSERT_EQUALS(a[1], 0);
    TS_ASSERT_EQUALS(a[2], 0);
    TS_ASSERT_EQUALS(a[3], 0);
    TS_ASSERT_EQUALS(a[4], 0);
    TS_ASSERT_EQUALS(a[5], 0);
    TS_ASSERT_EQUALS(a[6], 0);
    TS_ASSERT_EQUALS(a[7], 0);

    afl::bits::UInt64BE::pack(a, 0xFFFFFFFFFFFFFFFFULL);
    TS_ASSERT_EQUALS(a[0], 0xFF);
    TS_ASSERT_EQUALS(a[1], 0xFF);
    TS_ASSERT_EQUALS(a[2], 0xFF);
    TS_ASSERT_EQUALS(a[3], 0xFF);
    TS_ASSERT_EQUALS(a[4], 0xFF);
    TS_ASSERT_EQUALS(a[5], 0xFF);
    TS_ASSERT_EQUALS(a[6], 0xFF);
    TS_ASSERT_EQUALS(a[7], 0xFF);

    afl::bits::UInt64BE::pack(a, 0x8000000000000000ULL);
    TS_ASSERT_EQUALS(a[0], 0x80);
    TS_ASSERT_EQUALS(a[1], 0x00);
    TS_ASSERT_EQUALS(a[2], 0x00);
    TS_ASSERT_EQUALS(a[3], 0x00);
    TS_ASSERT_EQUALS(a[4], 0x00);
    TS_ASSERT_EQUALS(a[5], 0x00);
    TS_ASSERT_EQUALS(a[6], 0x00);
    TS_ASSERT_EQUALS(a[7], 0x00);

    afl::bits::UInt64BE::pack(a, 0x123456789ABCDEF0ULL);
    TS_ASSERT_EQUALS(a[0], 0x12);
    TS_ASSERT_EQUALS(a[1], 0x34);
    TS_ASSERT_EQUALS(a[2], 0x56);
    TS_ASSERT_EQUALS(a[3], 0x78);
    TS_ASSERT_EQUALS(a[4], 0x9A);
    TS_ASSERT_EQUALS(a[5], 0xBC);
    TS_ASSERT_EQUALS(a[6], 0xDE);
    TS_ASSERT_EQUALS(a[7], 0xF0);

    afl::bits::UInt64BE::pack(a, 0x00000000000000FFULL);
    TS_ASSERT_EQUALS(a[0], 0x00);
    TS_ASSERT_EQUALS(a[1], 0x00);
    TS_ASSERT_EQUALS(a[2], 0x00);
    TS_ASSERT_EQUALS(a[3], 0x00);
    TS_ASSERT_EQUALS(a[4], 0x00);
    TS_ASSERT_EQUALS(a[5], 0x00);
    TS_ASSERT_EQUALS(a[6], 0x00);
    TS_ASSERT_EQUALS(a[7], 0xFF);
}
