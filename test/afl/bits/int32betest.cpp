/**
  *  \file test/afl/bits/int32betest.cpp
  *  \brief Test for afl::bits::Int32BE
  */

#include "afl/bits/int32be.hpp"
#include "afl/test/testrunner.hpp"

// Unpacking regular + border cases
AFL_TEST("afl.bits.Int32BE:unpack:01020304", a) {
    uint8_t a1[4] = { 1, 2, 3, 4 };
    a.checkEqual("", afl::bits::Int32BE::unpack(a1), 0x01020304);
}

AFL_TEST("afl.bits.Int32BE:unpack:00000000", a) {
    uint8_t a2[4] = { 0, 0, 0, 0 };
    a.checkEqual("", afl::bits::Int32BE::unpack(a2), 0);
}

AFL_TEST("afl.bits.Int32BE:unpack:FFFFFFFF", a) {
    uint8_t a3[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
    a.checkEqual("", afl::bits::Int32BE::unpack(a3), -1LL);
}

AFL_TEST("afl.bits.Int32BE:unpack:80000000", a) {
    uint8_t a4[4] = { 0x80, 0x00, 0x00, 0x00 };
    a.checkEqual("", afl::bits::Int32BE::unpack(a4), -0x80000000LL);
}

AFL_TEST("afl.bits.Int32BE:unpack:000000FF", a) {
    uint8_t a5[4] = { 0x00, 0x00, 0x00, 0xFF };
    a.checkEqual("", afl::bits::Int32BE::unpack(a5), 255);
}

AFL_TEST("afl.bits.Int32BE:unpack:00FF0000", a) {
    uint8_t a6[4] = { 0x00, 0xFF, 0x00, 0x00 };
    a.checkEqual("", afl::bits::Int32BE::unpack(a6), 0x00FF0000);
}

AFL_TEST("afl.bits.Int32BE:unpack:7FFFFFFFF", a) {
    uint8_t a7[4] = { 0x7F, 0xFF, 0xFF, 0xFF };
    a.checkEqual("", afl::bits::Int32BE::unpack(a7), 0x7FFFFFFFLL);
}

// Packing
AFL_TEST("afl.bits.Int32BE:pack:0", a) {
    uint8_t d[4];
    afl::bits::Int32BE::pack(d, 0);
    a.checkEqual("0", d[0], 0);
    a.checkEqual("1", d[1], 0);
    a.checkEqual("2", d[2], 0);
    a.checkEqual("3", d[3], 0);
}

AFL_TEST("afl.bits.Int32BE:pack:FFFF", a) {
    uint8_t d[4];
    afl::bits::Int32BE::pack(d, 0xFFFF);
    a.checkEqual("0", d[0], 0);
    a.checkEqual("1", d[1], 0);
    a.checkEqual("2", d[2], 0xFF);
    a.checkEqual("3", d[3], 0xFF);
}

AFL_TEST("afl.bits.Int32BE:pack:80000000", a) {
    uint8_t d[4];
    afl::bits::Int32BE::pack(d, static_cast<int32_t>(-0x80000000));
    a.checkEqual("0", d[0], 0x80);
    a.checkEqual("1", d[1], 0x00);
    a.checkEqual("2", d[2], 0x00);
    a.checkEqual("3", d[3], 0x00);
}

AFL_TEST("afl.bits.Int32BE:pack:12345678", a) {
    uint8_t d[4];
    afl::bits::Int32BE::pack(d, 0x12345678);
    a.checkEqual("0", d[0], 0x12);
    a.checkEqual("1", d[1], 0x34);
    a.checkEqual("2", d[2], 0x56);
    a.checkEqual("3", d[3], 0x78);
}

AFL_TEST("afl.bits.Int32BE:pack:255", a) {
    uint8_t d[4];
    afl::bits::Int32BE::pack(d, 255);
    a.checkEqual("0", d[0], 0x00);
    a.checkEqual("1", d[1], 0x00);
    a.checkEqual("2", d[2], 0x00);
    a.checkEqual("3", d[3], 0xFF);
}

AFL_TEST("afl.bits.Int32BE:pack:7FFFFFFF", a) {
    uint8_t d[4];
    afl::bits::Int32BE::pack(d, 0x7FFFFFFF);
    a.checkEqual("0", d[0], 0x7F);
    a.checkEqual("1", d[1], 0xFF);
    a.checkEqual("2", d[2], 0xFF);
    a.checkEqual("3", d[3], 0xFF);
}
