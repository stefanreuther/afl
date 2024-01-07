/**
  *  \file test/afl/bits/bitstest.cpp
  *  \brief Test for afl::bits::Bits
  */

#include "afl/bits/bits.hpp"
#include "afl/test/testrunner.hpp"

/** Test isPowerOfTwo(). */
AFL_TEST("afl.bits.Bits:isPowerOfTwo", a)
{
    a.check("1",  afl::bits::isPowerOfTwo(1));
    a.check("2",  afl::bits::isPowerOfTwo(2));
    a.check("3", !afl::bits::isPowerOfTwo(3));
    a.check("4",  afl::bits::isPowerOfTwo(4));
    a.check("5", !afl::bits::isPowerOfTwo(5));
    a.check("6", !afl::bits::isPowerOfTwo(6));
    a.check("7", !afl::bits::isPowerOfTwo(7));
    a.check("8",  afl::bits::isPowerOfTwo(8));
    a.check("9", !afl::bits::isPowerOfTwo(9));
    a.check("10", !afl::bits::isPowerOfTwo(10));
    a.check("11", !afl::bits::isPowerOfTwo(11));
    a.check("12", !afl::bits::isPowerOfTwo(12));
    a.check("13", !afl::bits::isPowerOfTwo(13));
    a.check("14", !afl::bits::isPowerOfTwo(14));
    a.check("15", !afl::bits::isPowerOfTwo(15));
    a.check("16",  afl::bits::isPowerOfTwo(16));
    a.check("17", !afl::bits::isPowerOfTwo(17));
    a.check("18", !afl::bits::isPowerOfTwo(18));
    a.check("19", !afl::bits::isPowerOfTwo(19));

    a.check("159", !afl::bits::isPowerOfTwo(159));
    a.check("160", !afl::bits::isPowerOfTwo(160));
    a.check("161", !afl::bits::isPowerOfTwo(161));

    a.check("169", !afl::bits::isPowerOfTwo(169));
    a.check("170", !afl::bits::isPowerOfTwo(170));
    a.check("171", !afl::bits::isPowerOfTwo(171));

    a.check("255", !afl::bits::isPowerOfTwo(255));
    a.check("256",  afl::bits::isPowerOfTwo(256));
    a.check("257", !afl::bits::isPowerOfTwo(257));
}

/** Test bitLog32() and bitLog64(). */
AFL_TEST("afl.bits.Bits:bitLog32", a)
{
    // 32-bit version
    a.checkEqual("0", afl::bits::bitLog32(0), -1);
    a.checkEqual("1", afl::bits::bitLog32(1), 0);
    a.checkEqual("2", afl::bits::bitLog32(2), 1);
    a.checkEqual("3", afl::bits::bitLog32(3), 1);
    a.checkEqual("4", afl::bits::bitLog32(4), 2);
    a.checkEqual("5", afl::bits::bitLog32(5), 2);
    a.checkEqual("6", afl::bits::bitLog32(6), 2);
    a.checkEqual("7", afl::bits::bitLog32(7), 2);
    a.checkEqual("8", afl::bits::bitLog32(8), 3);

    a.checkEqual("255", afl::bits::bitLog32(255), 7);
    a.checkEqual("256", afl::bits::bitLog32(256), 8);

    a.checkEqual("64k", afl::bits::bitLog32(65536), 16);

    a.checkEqual("4g", afl::bits::bitLog32(0xFFFFFFFF), 31);
}

AFL_TEST("afl.bits.Bits:bitLog64", a)
{
    // 64-bit version
    a.checkEqual("0", afl::bits::bitLog64(0), -1);
    a.checkEqual("1", afl::bits::bitLog64(1), 0);
    a.checkEqual("2", afl::bits::bitLog64(2), 1);
    a.checkEqual("3", afl::bits::bitLog64(3), 1);
    a.checkEqual("4", afl::bits::bitLog64(4), 2);
    a.checkEqual("5", afl::bits::bitLog64(5), 2);
    a.checkEqual("6", afl::bits::bitLog64(6), 2);
    a.checkEqual("7", afl::bits::bitLog64(7), 2);
    a.checkEqual("8", afl::bits::bitLog64(8), 3);

    a.checkEqual("255", afl::bits::bitLog64(255), 7);
    a.checkEqual("256", afl::bits::bitLog64(256), 8);

    a.checkEqual("64k", afl::bits::bitLog64(65536), 16);

    a.checkEqual("4g", afl::bits::bitLog64(0xFFFFFFFF), 31);
    a.checkEqual("2^48", afl::bits::bitLog64(0xFFFFFFFFFFFFULL), 47);
    a.checkEqual("2^64", afl::bits::bitLog64(0xFFFFFFFFFFFFFFFFULL), 63);
}

/** Test bitReverse8(), bitReverse16(), bitReverse32(), bitReverse64(). */
AFL_TEST("afl.bits.Bits:bitReverse8", a)
{
    a.checkEqual("0", afl::bits::bitReverse8(0),  0x00);
    a.checkEqual("1", afl::bits::bitReverse8(1),  0x80);
    a.checkEqual("2", afl::bits::bitReverse8(2),  0x40);
    a.checkEqual("3", afl::bits::bitReverse8(3),  0xC0);
    a.checkEqual("4", afl::bits::bitReverse8(4),  0x20);
    a.checkEqual("5", afl::bits::bitReverse8(5),  0xA0);
    a.checkEqual("6", afl::bits::bitReverse8(6),  0x60);
    a.checkEqual("7", afl::bits::bitReverse8(7),  0xE0);
    a.checkEqual("8", afl::bits::bitReverse8(8),  0x10);
    a.checkEqual("9", afl::bits::bitReverse8(9),  0x90);
    a.checkEqual("10", afl::bits::bitReverse8(10), 0x50);
    a.checkEqual("11", afl::bits::bitReverse8(11), 0xD0);
    a.checkEqual("12", afl::bits::bitReverse8(12), 0x30);
    a.checkEqual("13", afl::bits::bitReverse8(13), 0xB0);
    a.checkEqual("14", afl::bits::bitReverse8(14), 0x70);
    a.checkEqual("15", afl::bits::bitReverse8(15), 0xF0);
    a.checkEqual("0x12", afl::bits::bitReverse8(0x12), 0x48);
}

AFL_TEST("afl.bits.Bits:bitReverse16", a)
{
    a.checkEqual("1", afl::bits::bitReverse16(1), 0x8000);
    a.checkEqual("bit", afl::bits::bitReverse16(0x1234),     0x2C48);
}

AFL_TEST("afl.bits.Bits:bitReverse32", a)
{
    a.checkEqual("1", afl::bits::bitReverse32(1), 0x80000000U);
    a.checkEqual("big", afl::bits::bitReverse32(0x12345678), 0x1E6A2C48U);
}

AFL_TEST("afl.bits.Bits:bitReverse64", a) {
    a.checkEqual("1", afl::bits::bitReverse64(1), 0x8000000000000000ULL);
    a.checkEqual("big", afl::bits::bitReverse64(0x1020304050607080ULL), 0x010E060A020C0408ULL);
}

/** Test bitPop8(), bitPop16(), bitPop32(), bitPop64(). */
AFL_TEST("afl.bits.Bits:bitPop8", a)
{
    a.checkEqual("A", afl::bits::bitPop8(0),   0U);
    a.checkEqual("B", afl::bits::bitPop8(1),   1U);
    a.checkEqual("C", afl::bits::bitPop8(17),  2U);
    a.checkEqual("D", afl::bits::bitPop8(128), 1U);
    a.checkEqual("E", afl::bits::bitPop8(129), 2U);
    a.checkEqual("F", afl::bits::bitPop8(255), 8U);
}

AFL_TEST("afl.bits.Bits:bitPop16", a)
{
    a.checkEqual("A", afl::bits::bitPop16(0),     0U);
    a.checkEqual("B", afl::bits::bitPop16(1),     1U);
    a.checkEqual("C", afl::bits::bitPop16(17),    2U);
    a.checkEqual("D", afl::bits::bitPop16(128),   1U);
    a.checkEqual("E", afl::bits::bitPop16(129),   2U);
    a.checkEqual("F", afl::bits::bitPop16(255),   8U);
    a.checkEqual("G", afl::bits::bitPop16(256),   1U);
    a.checkEqual("H", afl::bits::bitPop16(32768), 1U);
    a.checkEqual("I", afl::bits::bitPop16(65535), 16U);
}

AFL_TEST("afl.bits.Bits:bitPop32", a)
{
    a.checkEqual("A", afl::bits::bitPop32(0),     0U);
    a.checkEqual("B", afl::bits::bitPop32(1),     1U);
    a.checkEqual("C", afl::bits::bitPop32(17),    2U);
    a.checkEqual("D", afl::bits::bitPop32(128),   1U);
    a.checkEqual("E", afl::bits::bitPop32(129),   2U);
    a.checkEqual("F", afl::bits::bitPop32(255),   8U);
    a.checkEqual("G", afl::bits::bitPop32(256),   1U);
    a.checkEqual("H", afl::bits::bitPop32(32768), 1U);
    a.checkEqual("I", afl::bits::bitPop32(65535), 16U);
    a.checkEqual("J", afl::bits::bitPop32(65536), 1U);
    a.checkEqual("K", afl::bits::bitPop32(0xFFFFFFFE), 31U);
    a.checkEqual("L", afl::bits::bitPop32(0xFFFFFFFF), 32U);
}

AFL_TEST("afl.bits.Bits:bitPop64", a)
{
    a.checkEqual("A", afl::bits::bitPop64(0),     0U);
    a.checkEqual("B", afl::bits::bitPop64(1),     1U);
    a.checkEqual("C", afl::bits::bitPop64(17),    2U);
    a.checkEqual("D", afl::bits::bitPop64(128),   1U);
    a.checkEqual("E", afl::bits::bitPop64(129),   2U);
    a.checkEqual("F", afl::bits::bitPop64(255),   8U);
    a.checkEqual("G", afl::bits::bitPop64(256),   1U);
    a.checkEqual("H", afl::bits::bitPop64(32768), 1U);
    a.checkEqual("I", afl::bits::bitPop64(65535), 16U);
    a.checkEqual("J", afl::bits::bitPop64(65536), 1U);
    a.checkEqual("K", afl::bits::bitPop64(0xFFFFFFFE), 31U);
    a.checkEqual("L", afl::bits::bitPop64(0xFFFFFFFF), 32U);
    a.checkEqual("M", afl::bits::bitPop64(0x8000000000000000ULL), 1U);
    a.checkEqual("N", afl::bits::bitPop64(0xFFFFFFFFFFFFFFFFULL), 64U);
}

AFL_TEST("afl.bits.Bits:bitPop", a)
{
    a.checkEqual("A", afl::bits::bitPop(uint8_t(255U)), 8U);
    a.checkEqual("B", afl::bits::bitPop(uint16_t(65535U)), 16U);
    a.checkEqual("C", afl::bits::bitPop(uint32_t(0xFFFFFFFFU)), 32U);
    a.checkEqual("D", afl::bits::bitPop(uint64_t(0xFFFFFFFFFFFFFFFFULL)), 64U);
}

