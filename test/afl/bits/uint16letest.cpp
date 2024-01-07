/**
  *  \file test/afl/bits/uint16letest.cpp
  *  \brief Test for afl::bits::UInt16LE
  */

#include "afl/bits/uint16le.hpp"
#include "afl/test/testrunner.hpp"

// Unpacking regular + border cases
AFL_TEST("afl.bits.UInt16LE:unpack:0201", a) {
    uint8_t a1[2] = { 1, 2 };
    a.checkEqual("", afl::bits::UInt16LE::unpack(a1), 0x0201);
}

AFL_TEST("afl.bits.UInt16LE:unpack:0000", a) {
    uint8_t a2[2] = { 0, 0 };
    a.checkEqual("", afl::bits::UInt16LE::unpack(a2), 0x0000);
}

AFL_TEST("afl.bits.UInt16LE:unpack:FFFF", a) {
    uint8_t a3[2] = { 0xFF, 0xFF };
    a.checkEqual("", afl::bits::UInt16LE::unpack(a3), 0xFFFF);
}

AFL_TEST("afl.bits.UInt16LE:unpack:8000", a) {
    uint8_t a4[2] = { 0x00, 0x80 };
    a.checkEqual("", afl::bits::UInt16LE::unpack(a4), 0x8000);
}

AFL_TEST("afl.bits.UInt16LE:unpack:00FF", a) {
    uint8_t a5[2] = { 0xFF, 0x00 };
    a.checkEqual("", afl::bits::UInt16LE::unpack(a5), 0x00FF);
}

    // Packing
AFL_TEST("afl.bits.UInt16LE:pack:0", a) {
    uint8_t d[2];
    afl::bits::UInt16LE::pack(d, 0);
    a.checkEqual("0", d[0], 0);
    a.checkEqual("1", d[1], 0);
}

AFL_TEST("afl.bits.UInt16LE:pack:FFFF", a) {
    uint8_t d[2];
    afl::bits::UInt16LE::pack(d, 0xFFFF);
    a.checkEqual("0", d[0], 0xFF);
    a.checkEqual("1", d[1], 0xFF);
}

AFL_TEST("afl.bits.UInt16LE:pack:8000", a) {
    uint8_t d[2];
    afl::bits::UInt16LE::pack(d, 0x8000);
    a.checkEqual("0", d[0], 0x00);
    a.checkEqual("1", d[1], 0x80);
}

AFL_TEST("afl.bits.UInt16LE:pack:1234", a) {
    uint8_t d[2];
    afl::bits::UInt16LE::pack(d, 0x1234);
    a.checkEqual("0", d[0], 0x34);
    a.checkEqual("1", d[1], 0x12);
}

AFL_TEST("afl.bits.UInt16LE:pack:00FF", a) {
    uint8_t d[2];
    afl::bits::UInt16LE::pack(d, 0x00FF);
    a.checkEqual("0", d[0], 0xFF);
    a.checkEqual("1", d[1], 0x00);
}
