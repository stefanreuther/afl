/**
  *  \file u/t_bits_bits.cpp
  *  \brief Test for afl/bits/bits.hpp
  */

#include "afl/bits/bits.hpp"

#include "u/t_bits.hpp"

/** Test isPowerOfTwo(). */
void
TestBitsBits::testPower()
{
    TS_ASSERT( afl::bits::isPowerOfTwo(1));
    TS_ASSERT( afl::bits::isPowerOfTwo(2));
    TS_ASSERT(!afl::bits::isPowerOfTwo(3));
    TS_ASSERT( afl::bits::isPowerOfTwo(4));
    TS_ASSERT(!afl::bits::isPowerOfTwo(5));
    TS_ASSERT(!afl::bits::isPowerOfTwo(6));
    TS_ASSERT(!afl::bits::isPowerOfTwo(7));
    TS_ASSERT( afl::bits::isPowerOfTwo(8));
    TS_ASSERT(!afl::bits::isPowerOfTwo(9));
    TS_ASSERT(!afl::bits::isPowerOfTwo(10));
    TS_ASSERT(!afl::bits::isPowerOfTwo(11));
    TS_ASSERT(!afl::bits::isPowerOfTwo(12));
    TS_ASSERT(!afl::bits::isPowerOfTwo(13));
    TS_ASSERT(!afl::bits::isPowerOfTwo(14));
    TS_ASSERT(!afl::bits::isPowerOfTwo(15));
    TS_ASSERT( afl::bits::isPowerOfTwo(16));
    TS_ASSERT(!afl::bits::isPowerOfTwo(17));
    TS_ASSERT(!afl::bits::isPowerOfTwo(18));
    TS_ASSERT(!afl::bits::isPowerOfTwo(19));

    TS_ASSERT(!afl::bits::isPowerOfTwo(159));
    TS_ASSERT(!afl::bits::isPowerOfTwo(160));
    TS_ASSERT(!afl::bits::isPowerOfTwo(161));

    TS_ASSERT(!afl::bits::isPowerOfTwo(169));
    TS_ASSERT(!afl::bits::isPowerOfTwo(170));
    TS_ASSERT(!afl::bits::isPowerOfTwo(171));

    TS_ASSERT(!afl::bits::isPowerOfTwo(255));
    TS_ASSERT( afl::bits::isPowerOfTwo(256));
    TS_ASSERT(!afl::bits::isPowerOfTwo(257));
}

/** Test bitLog32() and bitLog64(). */
void
TestBitsBits::testLog()
{
    // 32-bit version
    TS_ASSERT_EQUALS(afl::bits::bitLog32(0), -1);
    TS_ASSERT_EQUALS(afl::bits::bitLog32(1), 0);
    TS_ASSERT_EQUALS(afl::bits::bitLog32(2), 1);
    TS_ASSERT_EQUALS(afl::bits::bitLog32(3), 1);
    TS_ASSERT_EQUALS(afl::bits::bitLog32(4), 2);
    TS_ASSERT_EQUALS(afl::bits::bitLog32(5), 2);
    TS_ASSERT_EQUALS(afl::bits::bitLog32(6), 2);
    TS_ASSERT_EQUALS(afl::bits::bitLog32(7), 2);
    TS_ASSERT_EQUALS(afl::bits::bitLog32(8), 3);

    TS_ASSERT_EQUALS(afl::bits::bitLog32(255), 7);
    TS_ASSERT_EQUALS(afl::bits::bitLog32(256), 8);

    TS_ASSERT_EQUALS(afl::bits::bitLog32(65536), 16);

    TS_ASSERT_EQUALS(afl::bits::bitLog32(0xFFFFFFFF), 31);

    // 64-bit version
    TS_ASSERT_EQUALS(afl::bits::bitLog64(0), -1);
    TS_ASSERT_EQUALS(afl::bits::bitLog64(1), 0);
    TS_ASSERT_EQUALS(afl::bits::bitLog64(2), 1);
    TS_ASSERT_EQUALS(afl::bits::bitLog64(3), 1);
    TS_ASSERT_EQUALS(afl::bits::bitLog64(4), 2);
    TS_ASSERT_EQUALS(afl::bits::bitLog64(5), 2);
    TS_ASSERT_EQUALS(afl::bits::bitLog64(6), 2);
    TS_ASSERT_EQUALS(afl::bits::bitLog64(7), 2);
    TS_ASSERT_EQUALS(afl::bits::bitLog64(8), 3);

    TS_ASSERT_EQUALS(afl::bits::bitLog64(255), 7);
    TS_ASSERT_EQUALS(afl::bits::bitLog64(256), 8);

    TS_ASSERT_EQUALS(afl::bits::bitLog64(65536), 16);

    TS_ASSERT_EQUALS(afl::bits::bitLog64(0xFFFFFFFF), 31);
    TS_ASSERT_EQUALS(afl::bits::bitLog64(0xFFFFFFFFFFFFULL), 47);
    TS_ASSERT_EQUALS(afl::bits::bitLog64(0xFFFFFFFFFFFFFFFFULL), 63);
}

/** Test bitReverse8(), bitReverse16(), bitReverse32(), bitReverse64(). */
void
TestBitsBits::testReverse()
{
    TS_ASSERT_EQUALS(afl::bits::bitReverse8(1),  0x80);
    TS_ASSERT_EQUALS(afl::bits::bitReverse16(1), 0x8000);
    TS_ASSERT_EQUALS(afl::bits::bitReverse32(1), 0x80000000U);
    TS_ASSERT_EQUALS(afl::bits::bitReverse64(1), 0x8000000000000000ULL);

    TS_ASSERT_EQUALS(afl::bits::bitReverse8(0x12),        0x48);
    TS_ASSERT_EQUALS(afl::bits::bitReverse16(0x1234),     0x2C48);
    TS_ASSERT_EQUALS(afl::bits::bitReverse32(0x12345678), 0x1E6A2C48U);
    TS_ASSERT_EQUALS(afl::bits::bitReverse64(0x1020304050607080ULL), 0x010E060A020C0408ULL);

    TS_ASSERT_EQUALS(afl::bits::bitReverse8(0),  0x00);
    TS_ASSERT_EQUALS(afl::bits::bitReverse8(1),  0x80);
    TS_ASSERT_EQUALS(afl::bits::bitReverse8(2),  0x40);
    TS_ASSERT_EQUALS(afl::bits::bitReverse8(3),  0xC0);
    TS_ASSERT_EQUALS(afl::bits::bitReverse8(4),  0x20);
    TS_ASSERT_EQUALS(afl::bits::bitReverse8(5),  0xA0);
    TS_ASSERT_EQUALS(afl::bits::bitReverse8(6),  0x60);
    TS_ASSERT_EQUALS(afl::bits::bitReverse8(7),  0xE0);
    TS_ASSERT_EQUALS(afl::bits::bitReverse8(8),  0x10);
    TS_ASSERT_EQUALS(afl::bits::bitReverse8(9),  0x90);
    TS_ASSERT_EQUALS(afl::bits::bitReverse8(10), 0x50);
    TS_ASSERT_EQUALS(afl::bits::bitReverse8(11), 0xD0);
    TS_ASSERT_EQUALS(afl::bits::bitReverse8(12), 0x30);
    TS_ASSERT_EQUALS(afl::bits::bitReverse8(13), 0xB0);
    TS_ASSERT_EQUALS(afl::bits::bitReverse8(14), 0x70);
    TS_ASSERT_EQUALS(afl::bits::bitReverse8(15), 0xF0);
}

/** Test bitPop8(), bitPop16(), bitPop32(), bitPop64(). */
void
TestBitsBits::testBitPop()
{
    TS_ASSERT_EQUALS(afl::bits::bitPop8(0),   0U);
    TS_ASSERT_EQUALS(afl::bits::bitPop8(1),   1U);
    TS_ASSERT_EQUALS(afl::bits::bitPop8(17),  2U);
    TS_ASSERT_EQUALS(afl::bits::bitPop8(128), 1U);
    TS_ASSERT_EQUALS(afl::bits::bitPop8(129), 2U);
    TS_ASSERT_EQUALS(afl::bits::bitPop8(255), 8U);

    TS_ASSERT_EQUALS(afl::bits::bitPop16(0),     0U);
    TS_ASSERT_EQUALS(afl::bits::bitPop16(1),     1U);
    TS_ASSERT_EQUALS(afl::bits::bitPop16(17),    2U);
    TS_ASSERT_EQUALS(afl::bits::bitPop16(128),   1U);
    TS_ASSERT_EQUALS(afl::bits::bitPop16(129),   2U);
    TS_ASSERT_EQUALS(afl::bits::bitPop16(255),   8U);
    TS_ASSERT_EQUALS(afl::bits::bitPop16(256),   1U);
    TS_ASSERT_EQUALS(afl::bits::bitPop16(32768), 1U);
    TS_ASSERT_EQUALS(afl::bits::bitPop16(65535), 16U);

    TS_ASSERT_EQUALS(afl::bits::bitPop32(0),     0U);
    TS_ASSERT_EQUALS(afl::bits::bitPop32(1),     1U);
    TS_ASSERT_EQUALS(afl::bits::bitPop32(17),    2U);
    TS_ASSERT_EQUALS(afl::bits::bitPop32(128),   1U);
    TS_ASSERT_EQUALS(afl::bits::bitPop32(129),   2U);
    TS_ASSERT_EQUALS(afl::bits::bitPop32(255),   8U);
    TS_ASSERT_EQUALS(afl::bits::bitPop32(256),   1U);
    TS_ASSERT_EQUALS(afl::bits::bitPop32(32768), 1U);
    TS_ASSERT_EQUALS(afl::bits::bitPop32(65535), 16U);
    TS_ASSERT_EQUALS(afl::bits::bitPop32(65536), 1U);
    TS_ASSERT_EQUALS(afl::bits::bitPop32(0xFFFFFFFE), 31U);
    TS_ASSERT_EQUALS(afl::bits::bitPop32(0xFFFFFFFF), 32U);

    TS_ASSERT_EQUALS(afl::bits::bitPop64(0),     0U);
    TS_ASSERT_EQUALS(afl::bits::bitPop64(1),     1U);
    TS_ASSERT_EQUALS(afl::bits::bitPop64(17),    2U);
    TS_ASSERT_EQUALS(afl::bits::bitPop64(128),   1U);
    TS_ASSERT_EQUALS(afl::bits::bitPop64(129),   2U);
    TS_ASSERT_EQUALS(afl::bits::bitPop64(255),   8U);
    TS_ASSERT_EQUALS(afl::bits::bitPop64(256),   1U);
    TS_ASSERT_EQUALS(afl::bits::bitPop64(32768), 1U);
    TS_ASSERT_EQUALS(afl::bits::bitPop64(65535), 16U);
    TS_ASSERT_EQUALS(afl::bits::bitPop64(65536), 1U);
    TS_ASSERT_EQUALS(afl::bits::bitPop64(0xFFFFFFFE), 31U);
    TS_ASSERT_EQUALS(afl::bits::bitPop64(0xFFFFFFFF), 32U);
    TS_ASSERT_EQUALS(afl::bits::bitPop64(0x8000000000000000ULL), 1U);
    TS_ASSERT_EQUALS(afl::bits::bitPop64(0xFFFFFFFFFFFFFFFFULL), 64U);

    TS_ASSERT_EQUALS(afl::bits::bitPop(uint8_t(255U)), 8U);
    TS_ASSERT_EQUALS(afl::bits::bitPop(uint16_t(65535U)), 16U);
    TS_ASSERT_EQUALS(afl::bits::bitPop(uint32_t(0xFFFFFFFFU)), 32U);
    TS_ASSERT_EQUALS(afl::bits::bitPop(uint64_t(0xFFFFFFFFFFFFFFFFULL)), 64U);
}

