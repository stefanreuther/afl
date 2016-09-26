/**
  *  \file u/t_bits_pack.cpp
  *  \brief Test for afl/bits/pack.hpp
  */

#include "afl/bits/pack.hpp"

#include "u/t_bits.hpp"
#include "afl/bits/uint16le.hpp"

/** Test packArray(). */
void
TestBitsPack::testPackArray()
{
    static const uint16_t one[] = { 0x1234 };
    static const uint16_t two[] = { 0x7766, 0x5544 };

    uint8_t a2[2] = { 7,7 };
    uint8_t a3[3] = { 8,8,8 };
    uint8_t a4[4] = { 9,9,9,9 };

    afl::bits::packArray<afl::bits::UInt16LE>(a2, one);
    TS_ASSERT_EQUALS(a2[0], 0x34);
    TS_ASSERT_EQUALS(a2[1], 0x12);

    afl::bits::packArray<afl::bits::UInt16LE>(a2, two);
    TS_ASSERT_EQUALS(a2[0], 0x66);
    TS_ASSERT_EQUALS(a2[1], 0x77);

    afl::bits::packArray<afl::bits::UInt16LE>(a3, two);
    TS_ASSERT_EQUALS(a3[0], 0x66);
    TS_ASSERT_EQUALS(a3[1], 0x77);
    TS_ASSERT_EQUALS(a3[2], 0);         // partial elements are zeroed

    afl::bits::packArray<afl::bits::UInt16LE>(a4, two);
    TS_ASSERT_EQUALS(a4[0], 0x66);
    TS_ASSERT_EQUALS(a4[1], 0x77);
    TS_ASSERT_EQUALS(a4[2], 0x44);
    TS_ASSERT_EQUALS(a4[3], 0x55);
}

/** Test unpackArray(). */
void
TestBitsPack::testUnpackArray()
{
    static const uint8_t one[] = { 0x12, 0x34 };
    static const uint8_t two[] = { 0x54, 0x32, 0x10, 0xFE };

    // Unpack one
    TS_ASSERT_EQUALS(afl::bits::UInt16LE::unpack(one), 0x3412);

    // Unpack one from an array
    const afl::bits::UInt16LE::Bytes_t* bytes = afl::base::Memory<const uint8_t>(two).eatN<2>();
    TS_ASSERT(bytes);
    TS_ASSERT_EQUALS(afl::bits::UInt16LE::unpack(*bytes), 0x3254);

    // Unpack whole array
    uint16_t a2[2] = { 7, 8 };
    afl::bits::unpackArray<afl::bits::UInt16LE>(a2, two);
    TS_ASSERT_EQUALS(a2[0], 0x3254);
    TS_ASSERT_EQUALS(a2[1], 0xFE10);

    // Unpack partial
    afl::bits::unpackArray<afl::bits::UInt16LE>(a2, one);
    TS_ASSERT_EQUALS(a2[0], 0x3412);
    TS_ASSERT_EQUALS(a2[1], 0);

    // Unpack partial
    static const uint8_t half[] = { 0x11, 0x22, 0x33 };
    afl::bits::unpackArray<afl::bits::UInt16LE>(a2, half);
    TS_ASSERT_EQUALS(a2[0], 0x2211);
    TS_ASSERT_EQUALS(a2[1], 0);
}
