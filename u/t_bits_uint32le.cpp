/**
  *  \file u/t_bits_uint32le.cpp
  *  \brief Test for afl::bits::UInt32LE
  */

#include "afl/bits/uint32le.hpp"
#include "u/t_bits.hpp"

/** Simple tests. */
void
TestBitsUInt32LE::testIt()
{
    // Unpacking regular + border cases
    uint8_t a1[4] = { 1, 2, 3, 4 };
    TS_ASSERT_EQUALS(afl::bits::UInt32LE::unpack(a1), 0x04030201U);

    uint8_t a2[4] = { 0, 0, 0, 0 };
    TS_ASSERT_EQUALS(afl::bits::UInt32LE::unpack(a2), 0x00000000U);

    uint8_t a3[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
    TS_ASSERT_EQUALS(afl::bits::UInt32LE::unpack(a3), 0xFFFFFFFFU);

    uint8_t a4[4] = { 0x00, 0x00, 0x00, 0x80 };
    TS_ASSERT_EQUALS(afl::bits::UInt32LE::unpack(a4), 0x80000000U);

    uint8_t a5[4] = { 0xFF, 0x00, 0x00, 0x00 };
    TS_ASSERT_EQUALS(afl::bits::UInt32LE::unpack(a5), 0x000000FFU);

    uint8_t a6[4] = { 0x00, 0x00, 0xFF, 0x00 };
    TS_ASSERT_EQUALS(afl::bits::UInt32LE::unpack(a6), 0x00FF0000U);

    // Packing
    uint8_t a[4];
    afl::bits::UInt32LE::pack(a, 0);
    TS_ASSERT_EQUALS(a[0], 0);
    TS_ASSERT_EQUALS(a[1], 0);
    TS_ASSERT_EQUALS(a[2], 0);
    TS_ASSERT_EQUALS(a[3], 0);

    afl::bits::UInt32LE::pack(a, 0xFFFF);
    TS_ASSERT_EQUALS(a[0], 0xFF);
    TS_ASSERT_EQUALS(a[1], 0xFF);
    TS_ASSERT_EQUALS(a[2], 0);
    TS_ASSERT_EQUALS(a[3], 0);

    afl::bits::UInt32LE::pack(a, 0x80000000);
    TS_ASSERT_EQUALS(a[0], 0x00);
    TS_ASSERT_EQUALS(a[1], 0x00);
    TS_ASSERT_EQUALS(a[2], 0x00);
    TS_ASSERT_EQUALS(a[3], 0x80);

    afl::bits::UInt32LE::pack(a, 0x12345678);
    TS_ASSERT_EQUALS(a[0], 0x78);
    TS_ASSERT_EQUALS(a[1], 0x56);
    TS_ASSERT_EQUALS(a[2], 0x34);
    TS_ASSERT_EQUALS(a[3], 0x12);

    afl::bits::UInt32LE::pack(a, 0x000000FF);
    TS_ASSERT_EQUALS(a[0], 0xFF);
    TS_ASSERT_EQUALS(a[1], 0x00);
    TS_ASSERT_EQUALS(a[2], 0x00);
    TS_ASSERT_EQUALS(a[3], 0x00);
}
