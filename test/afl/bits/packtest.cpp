/**
  *  \file test/afl/bits/packtest.cpp
  *  \brief Test for afl::bits::Pack
  */

#include "afl/bits/pack.hpp"
#include "afl/test/testrunner.hpp"

#include "afl/bits/uint16le.hpp"

/** Test packArray(). */
AFL_TEST("afl.bits.Pack:packArray", a)
{
    static const uint16_t one[] = { 0x1234 };
    static const uint16_t two[] = { 0x7766, 0x5544 };

    uint8_t a2[2] = { 7,7 };
    uint8_t a3[3] = { 8,8,8 };
    uint8_t a4[4] = { 9,9,9,9 };

    afl::bits::packArray<afl::bits::UInt16LE>(a2, one);
    a.checkEqual("one 0/2", a2[0], 0x34);
    a.checkEqual("one 1/2", a2[1], 0x12);

    afl::bits::packArray<afl::bits::UInt16LE>(a2, two);
    a.checkEqual("two 0/2", a2[0], 0x66);
    a.checkEqual("two 1/2", a2[1], 0x77);

    afl::bits::packArray<afl::bits::UInt16LE>(a3, two);
    a.checkEqual("two 0/3", a3[0], 0x66);
    a.checkEqual("two 1/3", a3[1], 0x77);
    a.checkEqual("two 2/3", a3[2], 0);         // partial elements are zeroed

    afl::bits::packArray<afl::bits::UInt16LE>(a4, two);
    a.checkEqual("two 0/4", a4[0], 0x66);
    a.checkEqual("two 1/4", a4[1], 0x77);
    a.checkEqual("two 2/4", a4[2], 0x44);
    a.checkEqual("two 3/4", a4[3], 0x55);
}

/** Test unpackArray(). */
AFL_TEST("afl.bits.Pack:unpackArray", a)
{
    static const uint8_t one[] = { 0x12, 0x34 };
    static const uint8_t two[] = { 0x54, 0x32, 0x10, 0xFE };

    // Unpack one
    a.checkEqual("one", afl::bits::UInt16LE::unpack(one), 0x3412);

    // Unpack one from an array
    const afl::bits::UInt16LE::Bytes_t* bytes = afl::base::Memory<const uint8_t>(two).eatN<2>();
    a.checkNonNull("one from array, bytes", bytes);
    a.checkEqual("one from array. value", afl::bits::UInt16LE::unpack(*bytes), 0x3254);

    // Unpack whole array
    uint16_t a2[2] = { 7, 8 };
    afl::bits::unpackArray<afl::bits::UInt16LE>(a2, two);
    a.checkEqual("whole array 0", a2[0], 0x3254);
    a.checkEqual("whole array 1", a2[1], 0xFE10);

    // Unpack partial
    afl::bits::unpackArray<afl::bits::UInt16LE>(a2, one);
    a.checkEqual("partial array 0", a2[0], 0x3412);
    a.checkEqual("partial array 1", a2[1], 0);

    afl::bits::unpackArray<afl::bits::UInt16LE>(a2, one, 77);
    a.checkEqual("partial pad 0", a2[0], 0x3412);
    a.checkEqual("partial pad 1", a2[1], 77);

    // Unpack partial
    static const uint8_t half[] = { 0x11, 0x22, 0x33 };
    afl::bits::unpackArray<afl::bits::UInt16LE>(a2, half);
    a.checkEqual("half 0", a2[0], 0x2211);
    a.checkEqual("half 1", a2[1], 0);

    afl::bits::unpackArray<afl::bits::UInt16LE>(a2, half, 99);
    a.checkEqual("half pad 0", a2[0], 0x2211);
    a.checkEqual("half pad 1", a2[1], 99);
}
