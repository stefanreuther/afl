/**
  *  \file test/afl/bits/int32letest.cpp
  *  \brief Test for afl::bits::Int32LE
  */

#include "afl/bits/int32le.hpp"
#include "afl/test/testrunner.hpp"

// Unpacking regular + border cases
AFL_TEST("afl.bits.Int32LE:unpack:04030201", a) {
    uint8_t a1[4] = { 1, 2, 3, 4 };
    a.checkEqual("", afl::bits::Int32LE::unpack(a1), 0x04030201);
}

AFL_TEST("afl.bits.Int32LE:unpack:00000000", a) {
    uint8_t a2[4] = { 0, 0, 0, 0 };
    a.checkEqual("", afl::bits::Int32LE::unpack(a2), 0);
}

AFL_TEST("afl.bits.Int32LE:unpack:FFFFFFFF", a) {
    uint8_t a3[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
    a.checkEqual("", afl::bits::Int32LE::unpack(a3), -1LL);
}

AFL_TEST("afl.bits.Int32LE:unpack:80000000", a) {
    uint8_t a4[4] = { 0x00, 0x00, 0x00, 0x80 };
    a.checkEqual("", afl::bits::Int32LE::unpack(a4), -0x80000000LL);
}

AFL_TEST("afl.bits.Int32LE:unpack:000000FF", a) {
    uint8_t a5[4] = { 0xFF, 0x00, 0x00, 0x00 };
    a.checkEqual("", afl::bits::Int32LE::unpack(a5), 255);
}

AFL_TEST("afl.bits.Int32LE:unpack:00FF0000", a) {
    uint8_t a6[4] = { 0x00, 0x00, 0xFF, 0x00 };
    a.checkEqual("", afl::bits::Int32LE::unpack(a6), 0x00FF0000);
}

AFL_TEST("afl.bits.Int32LE:unpack:7FFFFFFF", a) {
    uint8_t a7[4] = { 0xFF, 0xFF, 0xFF, 0x7F };
    a.checkEqual("", afl::bits::Int32LE::unpack(a7), 0x7FFFFFFFLL);
}

// Packing
AFL_TEST("afl.bits.Int32LE:pack:0", a) {
    uint8_t d[4];
    afl::bits::Int32LE::pack(d, 0);
    a.checkEqual("", d[0], 0);
    a.checkEqual("", d[1], 0);
    a.checkEqual("", d[2], 0);
    a.checkEqual("", d[3], 0);
}

AFL_TEST("afl.bits.Int32LE:pack:FFFF", a) {
    uint8_t d[4];
    afl::bits::Int32LE::pack(d, 0xFFFF);
    a.checkEqual("0", d[0], 0xFF);
    a.checkEqual("1", d[1], 0xFF);
    a.checkEqual("2", d[2], 0);
    a.checkEqual("3", d[3], 0);
}

AFL_TEST("afl.bits.Int32LE:pack:80000000", a) {
    uint8_t d[4];
    afl::bits::Int32LE::pack(d, static_cast<int32_t>(-0x80000000));
    a.checkEqual("0", d[0], 0x00);
    a.checkEqual("1", d[1], 0x00);
    a.checkEqual("2", d[2], 0x00);
    a.checkEqual("3", d[3], 0x80);
}

AFL_TEST("afl.bits.Int32LE:pack:12345678", a) {
    uint8_t d[4];
    afl::bits::Int32LE::pack(d, 0x12345678);
    a.checkEqual("0", d[0], 0x78);
    a.checkEqual("1", d[1], 0x56);
    a.checkEqual("2", d[2], 0x34);
    a.checkEqual("3", d[3], 0x12);
}

AFL_TEST("afl.bits.Int32LE:pack:255", a) {
    uint8_t d[4];
    afl::bits::Int32LE::pack(d, 255);
    a.checkEqual("0", d[0], 0xFF);
    a.checkEqual("1", d[1], 0x00);
    a.checkEqual("2", d[2], 0x00);
    a.checkEqual("3", d[3], 0x00);
}

AFL_TEST("afl.bits.Int32LE:pack:7FFFFFFF", a) {
    uint8_t d[4];
    afl::bits::Int32LE::pack(d, 0x7FFFFFFF);
    a.checkEqual("0", d[0], 0xFF);
    a.checkEqual("1", d[1], 0xFF);
    a.checkEqual("2", d[2], 0xFF);
    a.checkEqual("3", d[3], 0x7F);
}
