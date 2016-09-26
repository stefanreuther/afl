/**
  *  \file u/t_bits_int32be.cpp
  *  \brief Test for afl::bits::Int32BE
  */

#include "afl/bits/int32be.hpp"
#include "u/t_bits.hpp"

/** Simple tests. */
void
TestBitsInt32BE::testIt()
{
    // Unpacking regular + border cases
    uint8_t a1[4] = { 1, 2, 3, 4 };
    TS_ASSERT_EQUALS(afl::bits::Int32BE::unpack(a1), 0x01020304);

    uint8_t a2[4] = { 0, 0, 0, 0 };
    TS_ASSERT_EQUALS(afl::bits::Int32BE::unpack(a2), 0);

    uint8_t a3[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
    TS_ASSERT_EQUALS(afl::bits::Int32BE::unpack(a3), -1LL);

    uint8_t a4[4] = { 0x80, 0x00, 0x00, 0x00 };
    TS_ASSERT_EQUALS(afl::bits::Int32BE::unpack(a4), -0x80000000LL);

    uint8_t a5[4] = { 0x00, 0x00, 0x00, 0xFF };
    TS_ASSERT_EQUALS(afl::bits::Int32BE::unpack(a5), 255);

    uint8_t a6[4] = { 0x00, 0xFF, 0x00, 0x00 };
    TS_ASSERT_EQUALS(afl::bits::Int32BE::unpack(a6), 0x00FF0000);

    uint8_t a7[4] = { 0x7F, 0xFF, 0xFF, 0xFF };
    TS_ASSERT_EQUALS(afl::bits::Int32BE::unpack(a7), 0x7FFFFFFFLL);

    // Packing
    uint8_t a[4];
    afl::bits::Int32BE::pack(a, 0);
    TS_ASSERT_EQUALS(a[0], 0);
    TS_ASSERT_EQUALS(a[1], 0);
    TS_ASSERT_EQUALS(a[2], 0);
    TS_ASSERT_EQUALS(a[3], 0);

    afl::bits::Int32BE::pack(a, 0xFFFF);
    TS_ASSERT_EQUALS(a[0], 0);
    TS_ASSERT_EQUALS(a[1], 0);
    TS_ASSERT_EQUALS(a[2], 0xFF);
    TS_ASSERT_EQUALS(a[3], 0xFF);

    afl::bits::Int32BE::pack(a, -0x80000000);
    TS_ASSERT_EQUALS(a[0], 0x80);
    TS_ASSERT_EQUALS(a[1], 0x00);
    TS_ASSERT_EQUALS(a[2], 0x00);
    TS_ASSERT_EQUALS(a[3], 0x00);

    afl::bits::Int32BE::pack(a, 0x12345678);
    TS_ASSERT_EQUALS(a[0], 0x12);
    TS_ASSERT_EQUALS(a[1], 0x34);
    TS_ASSERT_EQUALS(a[2], 0x56);
    TS_ASSERT_EQUALS(a[3], 0x78);

    afl::bits::Int32BE::pack(a, 255);
    TS_ASSERT_EQUALS(a[0], 0x00);
    TS_ASSERT_EQUALS(a[1], 0x00);
    TS_ASSERT_EQUALS(a[2], 0x00);
    TS_ASSERT_EQUALS(a[3], 0xFF);

    afl::bits::Int32BE::pack(a, 0x7FFFFFFF);
    TS_ASSERT_EQUALS(a[0], 0x7F);
    TS_ASSERT_EQUALS(a[1], 0xFF);
    TS_ASSERT_EQUALS(a[2], 0xFF);
    TS_ASSERT_EQUALS(a[3], 0xFF);
}
