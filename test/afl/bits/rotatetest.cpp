/**
  *  \file test/afl/bits/rotatetest.cpp
  *  \brief Test for afl::bits::Rotate
  */

#include "afl/bits/rotate.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.bits.Rotate:8", a) {
    for (int i = 0; i < 255; ++i) {
        a.checkEqual("01", afl::bits::rotateLeft8(static_cast<uint8_t>(i), 0), i);
        a.checkEqual("02", afl::bits::rotateLeft8(static_cast<uint8_t>(i), 8), i);
        a.checkEqual("03", afl::bits::rotateLeft8(static_cast<uint8_t>(i), 800), i);
    }

    a.checkEqual("11", afl::bits::rotateLeft8(0x03, 1), 0x06);
    a.checkEqual("12", afl::bits::rotateLeft8(0x03, 2), 0x0C);
    a.checkEqual("13", afl::bits::rotateLeft8(0x03, 3), 0x18);
    a.checkEqual("14", afl::bits::rotateLeft8(0x03, 4), 0x30);
    a.checkEqual("15", afl::bits::rotateLeft8(0x03, 5), 0x60);
    a.checkEqual("16", afl::bits::rotateLeft8(0x03, 6), 0xC0);
    a.checkEqual("17", afl::bits::rotateLeft8(0x03, 7), 0x81);

    a.checkEqual("21", afl::bits::rotateLeft8(1, 1), 2);
    a.checkEqual("22", afl::bits::rotateLeft8(1, 2), 4);
    a.checkEqual("23", afl::bits::rotateLeft8(1, 4), 16);
    a.checkEqual("24", afl::bits::rotateLeft8(1, 8), 1);

    a.checkEqual("31", afl::bits::rotateRight8(1, 1), 128);
    a.checkEqual("32", afl::bits::rotateRight8(1, 2), 64);
    a.checkEqual("33", afl::bits::rotateRight8(1, 4), 16);
    a.checkEqual("34", afl::bits::rotateRight8(1, 8), 1);
}

AFL_TEST("afl.bits.Rotate:16", a) {
    for (int i = 0; i < 255; ++i) {
        uint16_t i16 = static_cast<uint16_t>(i);
        a.checkEqual("01", afl::bits::rotateLeft16(i16, 0), i);
        a.checkEqual("02", afl::bits::rotateLeft16(i16, 8), i << 8); // no overflow yet!
        a.checkEqual("03", afl::bits::rotateLeft16(i16, 16), i);
        a.checkEqual("04", afl::bits::rotateLeft16(i16, 800), i);
    }

    a.checkEqual("11", afl::bits::rotateLeft16(0x0003,  1), 0x0006);
    a.checkEqual("12", afl::bits::rotateLeft16(0x0003,  2), 0x000C);
    a.checkEqual("13", afl::bits::rotateLeft16(0x0003,  3), 0x0018);
    a.checkEqual("14", afl::bits::rotateLeft16(0x0003,  4), 0x0030);
    a.checkEqual("15", afl::bits::rotateLeft16(0x0003,  5), 0x0060);
    a.checkEqual("16", afl::bits::rotateLeft16(0x0003,  6), 0x00C0);
    a.checkEqual("17", afl::bits::rotateLeft16(0x0003,  7), 0x0180);
    a.checkEqual("18", afl::bits::rotateLeft16(0x0003,  8), 0x0300);
    a.checkEqual("19", afl::bits::rotateLeft16(0x0003,  9), 0x0600);
    a.checkEqual("1A", afl::bits::rotateLeft16(0x0003, 10), 0x0C00);
    a.checkEqual("1B", afl::bits::rotateLeft16(0x0003, 11), 0x1800);
    a.checkEqual("1C", afl::bits::rotateLeft16(0x0003, 12), 0x3000);
    a.checkEqual("1D", afl::bits::rotateLeft16(0x0003, 13), 0x6000);
    a.checkEqual("1E", afl::bits::rotateLeft16(0x0003, 14), 0xC000);
    a.checkEqual("1F", afl::bits::rotateLeft16(0x0003, 15), 0x8001);

    a.checkEqual("21", afl::bits::rotateLeft16(1, 1), 2);
    a.checkEqual("22", afl::bits::rotateLeft16(1, 2), 4);
    a.checkEqual("23", afl::bits::rotateLeft16(1, 4), 16);
    a.checkEqual("24", afl::bits::rotateLeft16(1, 15), 32768U);
    a.checkEqual("25", afl::bits::rotateLeft16(1, 16), 1);

    a.checkEqual("31", afl::bits::rotateRight16(1, 1), 32768U);
    a.checkEqual("32", afl::bits::rotateRight16(1, 2), 16384U);
    a.checkEqual("33", afl::bits::rotateRight16(1, 4), 4096U);
    a.checkEqual("34", afl::bits::rotateRight16(1, 15), 2);
    a.checkEqual("35", afl::bits::rotateRight16(1, 16), 1);
}

AFL_TEST("afl.bits.Rotate:32", a) {
    for (uint32_t i = 0; i < 255; ++i) {
        a.checkEqual("01", afl::bits::rotateLeft32(i, 0), i);
        a.checkEqual("02", afl::bits::rotateLeft32(i, 8), i << 8);    // no overflow yet!
        a.checkEqual("03", afl::bits::rotateLeft32(i, 16), i << 16);  // no overflow yet!
        a.checkEqual("04", afl::bits::rotateLeft32(i, 24), i << 24);  // no overflow yet!
        a.checkEqual("05", afl::bits::rotateLeft32(i, 32), i);
        a.checkEqual("06", afl::bits::rotateLeft32(i, 800), i);
    }

    a.checkEqual("11", afl::bits::rotateLeft32(0x00000003,  1), 0x00000006U);
    a.checkEqual("12", afl::bits::rotateLeft32(0x00000003,  2), 0x0000000CU);
    a.checkEqual("13", afl::bits::rotateLeft32(0x00000003,  3), 0x00000018U);
    a.checkEqual("14", afl::bits::rotateLeft32(0x00000003,  4), 0x00000030U);
    a.checkEqual("15", afl::bits::rotateLeft32(0x00000003,  5), 0x00000060U);
    a.checkEqual("16", afl::bits::rotateLeft32(0x00000003,  6), 0x000000C0U);
    a.checkEqual("17", afl::bits::rotateLeft32(0x00000003,  7), 0x00000180U);
    a.checkEqual("18", afl::bits::rotateLeft32(0x00000003,  8), 0x00000300U);
    a.checkEqual("19", afl::bits::rotateLeft32(0x00000003,  9), 0x00000600U);
    a.checkEqual("1A", afl::bits::rotateLeft32(0x00000003, 10), 0x00000C00U);
    a.checkEqual("1B", afl::bits::rotateLeft32(0x00000003, 11), 0x00001800U);
    a.checkEqual("1C", afl::bits::rotateLeft32(0x00000003, 12), 0x00003000U);
    a.checkEqual("1D", afl::bits::rotateLeft32(0x00000003, 13), 0x00006000U);
    a.checkEqual("1E", afl::bits::rotateLeft32(0x00000003, 14), 0x0000C000U);
    a.checkEqual("1F", afl::bits::rotateLeft32(0x00000003, 15), 0x00018000U);
    a.checkEqual("1B", afl::bits::rotateLeft32(0x00000003, 16), 0x00030000U);

    a.checkEqual("21", afl::bits::rotateLeft32(0x00000003, 30), 0xC0000000U);
    a.checkEqual("22", afl::bits::rotateLeft32(0x00000003, 31), 0x80000001U);

    a.checkEqual("31", afl::bits::rotateLeft32(1, 1), 2U);
    a.checkEqual("32", afl::bits::rotateLeft32(1, 16), 65536U);
    a.checkEqual("33", afl::bits::rotateLeft32(0x12345678, 4), 0x23456781U);
    a.checkEqual("34", afl::bits::rotateLeft32(0xFFFFFFFF, 7), 0xFFFFFFFFU);
    a.checkEqual("35", afl::bits::rotateLeft32(1, 32), 1U);

    a.checkEqual("41", afl::bits::rotateRight32(1, 1), 0x80000000U);
    a.checkEqual("42", afl::bits::rotateRight32(1, 16), 65536U);
    a.checkEqual("43", afl::bits::rotateRight32(0x12345678, 4), 0x81234567U);
    a.checkEqual("44", afl::bits::rotateRight32(0xFFFFFFFF, 7), 0xFFFFFFFFU);
    a.checkEqual("45", afl::bits::rotateRight32(1, 32), 1U);
}

AFL_TEST("afl.bits.Rotate:64", a) {
    for (uint64_t i = 0; i < 255; ++i) {
        a.checkEqual("01", afl::bits::rotateLeft64(i, 0), i);
        a.checkEqual("02", afl::bits::rotateLeft64(i, 8), i << 8);    // no overflow yet!
        a.checkEqual("03", afl::bits::rotateLeft64(i, 16), i << 16);  // no overflow yet!
        a.checkEqual("04", afl::bits::rotateLeft64(i, 24), i << 24);  // no overflow yet!
        a.checkEqual("05", afl::bits::rotateLeft64(i, 32), i << 32);
        a.checkEqual("06", afl::bits::rotateLeft64(i, 800), i << 32); // wraps around 12 times, then shift by 16
    }

    a.checkEqual("11", afl::bits::rotateLeft64(0x00000003,  1), 0x00000006ULL);
    a.checkEqual("12", afl::bits::rotateLeft64(0x00000003,  2), 0x0000000CULL);
    a.checkEqual("13", afl::bits::rotateLeft64(0x00000003,  3), 0x00000018ULL);
    a.checkEqual("14", afl::bits::rotateLeft64(0x00000003,  4), 0x00000030ULL);
    a.checkEqual("15", afl::bits::rotateLeft64(0x00000003,  5), 0x00000060ULL);
    a.checkEqual("16", afl::bits::rotateLeft64(0x00000003,  6), 0x000000C0ULL);
    a.checkEqual("17", afl::bits::rotateLeft64(0x00000003,  7), 0x00000180ULL);
    a.checkEqual("18", afl::bits::rotateLeft64(0x00000003,  8), 0x00000300ULL);
    a.checkEqual("19", afl::bits::rotateLeft64(0x00000003,  9), 0x00000600ULL);
    a.checkEqual("1A", afl::bits::rotateLeft64(0x00000003, 10), 0x00000C00ULL);
    a.checkEqual("1B", afl::bits::rotateLeft64(0x00000003, 11), 0x00001800ULL);
    a.checkEqual("1C", afl::bits::rotateLeft64(0x00000003, 12), 0x00003000ULL);
    a.checkEqual("1D", afl::bits::rotateLeft64(0x00000003, 13), 0x00006000ULL);
    a.checkEqual("1E", afl::bits::rotateLeft64(0x00000003, 14), 0x0000C000ULL);
    a.checkEqual("1F", afl::bits::rotateLeft64(0x00000003, 15), 0x00018000ULL);
    a.checkEqual("1B", afl::bits::rotateLeft64(0x00000003, 16), 0x00030000ULL);

    a.checkEqual("21", afl::bits::rotateLeft64(0x00000003, 30), 0x0C0000000ULL);
    a.checkEqual("22", afl::bits::rotateLeft64(0x00000003, 31), 0x180000000ULL);

    a.checkEqual("31", afl::bits::rotateLeft64(0x00000003, 62), 0xC000000000000000ULL);
    a.checkEqual("32", afl::bits::rotateLeft64(0x00000003, 63), 0x8000000000000001ULL);

    a.checkEqual("41", afl::bits::rotateLeft64(1, 1), 2ULL);
    a.checkEqual("42", afl::bits::rotateLeft64(1, 16), 65536ULL);
    a.checkEqual("43", afl::bits::rotateLeft64(0x123456789ABCDEF0ULL, 4), 0x23456789ABCDEF01ULL);
    a.checkEqual("44", afl::bits::rotateLeft64(0xFFFFFFFFFFFFFFFFULL, 7), 0xFFFFFFFFFFFFFFFFULL);
    a.checkEqual("45", afl::bits::rotateLeft64(1, 64), 1ULL);

    a.checkEqual("51", afl::bits::rotateRight64(1, 1), 0x8000000000000000ULL);
    a.checkEqual("52", afl::bits::rotateRight64(1, 48), 65536ULL);
    a.checkEqual("53", afl::bits::rotateRight64(0x123456789ABCDEF0ULL, 4), 0x0123456789ABCDEFULL);
    a.checkEqual("54", afl::bits::rotateRight64(0xFFFFFFFFFFFFFFFFULL, 7), 0xFFFFFFFFFFFFFFFFULL);
    a.checkEqual("55", afl::bits::rotateRight64(1, 64), 1ULL);
}
