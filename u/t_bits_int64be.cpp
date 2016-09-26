/**
  *  \file u/t_bits_int64be.cpp
  *  \brief Test for afl::bits::UInt64BE
  */

#include "afl/bits/int64be.hpp"
#include "u/t_bits.hpp"

/** Simple tests. */
void
TestBitsInt64BE::testIt()
{
    // Unpacking regular + border cases
    uint8_t a1[8] = { 8, 7, 6, 5, 4, 3, 2, 1 };
    TS_ASSERT_EQUALS(afl::bits::Int64BE::unpack(a1), 0x0807060504030201LL);

    uint8_t a2[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    TS_ASSERT_EQUALS(afl::bits::Int64BE::unpack(a2), 0);

    uint8_t a3[8] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    TS_ASSERT_EQUALS(afl::bits::Int64BE::unpack(a3), -1);

    uint8_t a4[8] = { 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    TS_ASSERT_EQUALS(afl::bits::Int64BE::unpack(a4), -0x7FFFFFFFFFFFFFFFLL-1);

    uint8_t a5[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF };
    TS_ASSERT_EQUALS(afl::bits::Int64BE::unpack(a5), 255);

    uint8_t a6[8] = { 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00 };
    TS_ASSERT_EQUALS(afl::bits::Int64BE::unpack(a6), 0x000000FF00000000LL);

    uint8_t a7[8] = { 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    TS_ASSERT_EQUALS(afl::bits::Int64BE::unpack(a7), 0x7FFFFFFFFFFFFFFFLL);

    // Packing
    uint8_t a[8];
    afl::bits::Int64BE::pack(a, 0);
    TS_ASSERT_EQUALS(a[0], 0);
    TS_ASSERT_EQUALS(a[1], 0);
    TS_ASSERT_EQUALS(a[2], 0);
    TS_ASSERT_EQUALS(a[3], 0);
    TS_ASSERT_EQUALS(a[4], 0);
    TS_ASSERT_EQUALS(a[5], 0);
    TS_ASSERT_EQUALS(a[6], 0);
    TS_ASSERT_EQUALS(a[7], 0);

    afl::bits::Int64BE::pack(a, -1);
    TS_ASSERT_EQUALS(a[0], 0xFF);
    TS_ASSERT_EQUALS(a[1], 0xFF);
    TS_ASSERT_EQUALS(a[2], 0xFF);
    TS_ASSERT_EQUALS(a[3], 0xFF);
    TS_ASSERT_EQUALS(a[4], 0xFF);
    TS_ASSERT_EQUALS(a[5], 0xFF);
    TS_ASSERT_EQUALS(a[6], 0xFF);
    TS_ASSERT_EQUALS(a[7], 0xFF);

    afl::bits::Int64BE::pack(a, -0x8000000000000000LL);
    TS_ASSERT_EQUALS(a[0], 0x80);
    TS_ASSERT_EQUALS(a[1], 0x00);
    TS_ASSERT_EQUALS(a[2], 0x00);
    TS_ASSERT_EQUALS(a[3], 0x00);
    TS_ASSERT_EQUALS(a[4], 0x00);
    TS_ASSERT_EQUALS(a[5], 0x00);
    TS_ASSERT_EQUALS(a[6], 0x00);
    TS_ASSERT_EQUALS(a[7], 0x00);

    afl::bits::Int64BE::pack(a, 0x123456789ABCDEF0LL);
    TS_ASSERT_EQUALS(a[0], 0x12);
    TS_ASSERT_EQUALS(a[1], 0x34);
    TS_ASSERT_EQUALS(a[2], 0x56);
    TS_ASSERT_EQUALS(a[3], 0x78);
    TS_ASSERT_EQUALS(a[4], 0x9A);
    TS_ASSERT_EQUALS(a[5], 0xBC);
    TS_ASSERT_EQUALS(a[6], 0xDE);
    TS_ASSERT_EQUALS(a[7], 0xF0);

    afl::bits::Int64BE::pack(a, 255);
    TS_ASSERT_EQUALS(a[0], 0x00);
    TS_ASSERT_EQUALS(a[1], 0x00);
    TS_ASSERT_EQUALS(a[2], 0x00);
    TS_ASSERT_EQUALS(a[3], 0x00);
    TS_ASSERT_EQUALS(a[4], 0x00);
    TS_ASSERT_EQUALS(a[5], 0x00);
    TS_ASSERT_EQUALS(a[6], 0x00);
    TS_ASSERT_EQUALS(a[7], 0xFF);
}
