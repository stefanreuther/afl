/**
  *  \file test/afl/bits/uint64letest.cpp
  *  \brief Test for afl::bits::UInt64LE
  */

#include "afl/bits/uint64le.hpp"
#include "afl/test/testrunner.hpp"

// Unpacking regular + border cases
AFL_TEST("afl.bits.UInt64LE:unpack:0807060504030201", a) {
    uint8_t a1[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    a.checkEqual("", afl::bits::UInt64LE::unpack(a1), 0x0807060504030201ULL);
}

AFL_TEST("afl.bits.UInt64LE:unpack:0000000000000000", a) {
    uint8_t a2[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    a.checkEqual("", afl::bits::UInt64LE::unpack(a2), 0x0000000000000000ULL);
}

AFL_TEST("afl.bits.UInt64LE:unpack:FFFFFFFFFFFFFFFF", a) {
    uint8_t a3[8] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    a.checkEqual("", afl::bits::UInt64LE::unpack(a3), 0xFFFFFFFFFFFFFFFFULL);
}

AFL_TEST("afl.bits.UInt64LE:unpack:8000000000000000", a) {
    uint8_t a4[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80 };
    a.checkEqual("", afl::bits::UInt64LE::unpack(a4), 0x8000000000000000ULL);
}

AFL_TEST("afl.bits.UInt64LE:unpack:00000000000000FF", a) {
    uint8_t a5[8] = { 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    a.checkEqual("", afl::bits::UInt64LE::unpack(a5), 0x00000000000000FFULL);
}

AFL_TEST("afl.bits.UInt64LE:unpack:000000FF00000000", a) {
    uint8_t a6[8] = { 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00 };
    a.checkEqual("", afl::bits::UInt64LE::unpack(a6), 0x000000FF00000000ULL);
}

// Packing
AFL_TEST("afl.bits.UInt64LE:pack:0", a) {
    uint8_t d[8];
    afl::bits::UInt64LE::pack(d, 0);
    a.checkEqual("0", d[0], 0);
    a.checkEqual("1", d[1], 0);
    a.checkEqual("2", d[2], 0);
    a.checkEqual("3", d[3], 0);
    a.checkEqual("4", d[4], 0);
    a.checkEqual("5", d[5], 0);
    a.checkEqual("6", d[6], 0);
    a.checkEqual("7", d[7], 0);
}

AFL_TEST("afl.bits.UInt64LE:pack:FFFFFFFFFFFFFFFF", a) {
    uint8_t d[8];
    afl::bits::UInt64LE::pack(d, 0xFFFFFFFFFFFFFFFFULL);
    a.checkEqual("0", d[0], 0xFF);
    a.checkEqual("1", d[1], 0xFF);
    a.checkEqual("2", d[2], 0xFF);
    a.checkEqual("3", d[3], 0xFF);
    a.checkEqual("4", d[4], 0xFF);
    a.checkEqual("5", d[5], 0xFF);
    a.checkEqual("6", d[6], 0xFF);
    a.checkEqual("7", d[7], 0xFF);
}

AFL_TEST("afl.bits.UInt64LE:pack:8000000000000000", a) {
    uint8_t d[8];
    afl::bits::UInt64LE::pack(d, 0x8000000000000000ULL);
    a.checkEqual("0", d[0], 0x00);
    a.checkEqual("1", d[1], 0x00);
    a.checkEqual("2", d[2], 0x00);
    a.checkEqual("3", d[3], 0x00);
    a.checkEqual("4", d[4], 0x00);
    a.checkEqual("5", d[5], 0x00);
    a.checkEqual("6", d[6], 0x00);
    a.checkEqual("7", d[7], 0x80);
}

AFL_TEST("afl.bits.UInt64LE:pack:123456789ABCDEF0", a) {
    uint8_t d[8];
    afl::bits::UInt64LE::pack(d, 0x123456789ABCDEF0ULL);
    a.checkEqual("0", d[0], 0xF0);
    a.checkEqual("1", d[1], 0xDE);
    a.checkEqual("2", d[2], 0xBC);
    a.checkEqual("3", d[3], 0x9A);
    a.checkEqual("4", d[4], 0x78);
    a.checkEqual("5", d[5], 0x56);
    a.checkEqual("6", d[6], 0x34);
    a.checkEqual("7", d[7], 0x12);
}

AFL_TEST("afl.bits.UInt64LE:pack:00000000000000FF", a) {
    uint8_t d[8];
    afl::bits::UInt64LE::pack(d, 0x00000000000000FFULL);
    a.checkEqual("0", d[0], 0xFF);
    a.checkEqual("1", d[1], 0x00);
    a.checkEqual("2", d[2], 0x00);
    a.checkEqual("3", d[3], 0x00);
    a.checkEqual("4", d[4], 0x00);
    a.checkEqual("5", d[5], 0x00);
    a.checkEqual("6", d[6], 0x00);
    a.checkEqual("7", d[7], 0x00);
}
