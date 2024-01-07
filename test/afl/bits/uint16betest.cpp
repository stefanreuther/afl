/**
  *  \file test/afl/bits/uint16betest.cpp
  *  \brief Test for afl::bits::UInt16BE
  */

#include "afl/bits/uint16be.hpp"
#include "afl/test/testrunner.hpp"

// Unpacking regular + border cases
AFL_TEST("afl.bits.UInt16BE:unpack:0201", a) {
    uint8_t a1[2] = { 2, 1 };
    a.checkEqual("", afl::bits::UInt16BE::unpack(a1), 0x0201);
}

AFL_TEST("afl.bits.UInt16BE:unpack:0000", a) {
    uint8_t a2[2] = { 0, 0 };
    a.checkEqual("", afl::bits::UInt16BE::unpack(a2), 0x0000);
}

AFL_TEST("afl.bits.UInt16BE:unpack:FFFF", a) {
    uint8_t a3[2] = { 0xFF, 0xFF };
    a.checkEqual("", afl::bits::UInt16BE::unpack(a3), 0xFFFF);
}

AFL_TEST("afl.bits.UInt16BE:unpack:8000", a) {
    uint8_t a4[2] = { 0x80, 0x00 };
    a.checkEqual("", afl::bits::UInt16BE::unpack(a4), 0x8000);
}

AFL_TEST("afl.bits.UInt16BE:unpack:00FF", a) {
    uint8_t a5[2] = { 0x00, 0xFF };
    a.checkEqual("", afl::bits::UInt16BE::unpack(a5), 0x00FF);
}

// Packing
AFL_TEST("afl.bits.UInt16BE:pack:0", a) {
    uint8_t d[2];
    afl::bits::UInt16BE::pack(d, 0);
    a.checkEqual("0", d[0], 0);
    a.checkEqual("1", d[1], 0);
}

AFL_TEST("afl.bits.UInt16BE:pack:FFFF", a) {
    uint8_t d[2];
    afl::bits::UInt16BE::pack(d, 0xFFFF);
    a.checkEqual("0", d[0], 0xFF);
    a.checkEqual("1", d[1], 0xFF);
}

AFL_TEST("afl.bits.UInt16BE:pack:8000", a) {
    uint8_t d[2];
    afl::bits::UInt16BE::pack(d, 0x8000);
    a.checkEqual("0", d[0], 0x80);
    a.checkEqual("1", d[1], 0x00);
}

AFL_TEST("afl.bits.UInt16BE:pack:1234", a) {
    uint8_t d[2];
    afl::bits::UInt16BE::pack(d, 0x1234);
    a.checkEqual("0", d[0], 0x12);
    a.checkEqual("1", d[1], 0x34);
}

AFL_TEST("afl.bits.UInt16BE:pack:00FF", a) {
    uint8_t d[2];
    afl::bits::UInt16BE::pack(d, 0x00FF);
    a.checkEqual("0", d[0], 0x00);
    a.checkEqual("1", d[1], 0xFF);
}
