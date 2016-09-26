/**
  *  \file u/t_bits_uint16le.cpp
  *  \brief Test for afl::bits::UInt16LE
  */

#include "afl/bits/uint16le.hpp"
#include "u/t_bits.hpp"

/** Simple tests. */
void
TestBitsUInt16LE::testIt()
{
    // Unpacking regular + border cases
    uint8_t a1[2] = { 1, 2 };
    TS_ASSERT_EQUALS(afl::bits::UInt16LE::unpack(a1), 0x0201);

    uint8_t a2[2] = { 0, 0 };
    TS_ASSERT_EQUALS(afl::bits::UInt16LE::unpack(a2), 0x0000);

    uint8_t a3[2] = { 0xFF, 0xFF };
    TS_ASSERT_EQUALS(afl::bits::UInt16LE::unpack(a3), 0xFFFF);

    uint8_t a4[2] = { 0x00, 0x80 };
    TS_ASSERT_EQUALS(afl::bits::UInt16LE::unpack(a4), 0x8000);

    uint8_t a5[2] = { 0xFF, 0x00 };
    TS_ASSERT_EQUALS(afl::bits::UInt16LE::unpack(a5), 0x00FF);

    // Packing
    uint8_t a[2];
    afl::bits::UInt16LE::pack(a, 0);
    TS_ASSERT_EQUALS(a[0], 0);
    TS_ASSERT_EQUALS(a[1], 0);

    afl::bits::UInt16LE::pack(a, 0xFFFF);
    TS_ASSERT_EQUALS(a[0], 0xFF);
    TS_ASSERT_EQUALS(a[1], 0xFF);

    afl::bits::UInt16LE::pack(a, 0x8000);
    TS_ASSERT_EQUALS(a[0], 0x00);
    TS_ASSERT_EQUALS(a[1], 0x80);

    afl::bits::UInt16LE::pack(a, 0x1234);
    TS_ASSERT_EQUALS(a[0], 0x34);
    TS_ASSERT_EQUALS(a[1], 0x12);

    afl::bits::UInt16LE::pack(a, 0x00FF);
    TS_ASSERT_EQUALS(a[0], 0xFF);
    TS_ASSERT_EQUALS(a[1], 0x00);
}
