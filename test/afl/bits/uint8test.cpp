/**
  *  \file test/afl/bits/uint8test.cpp
  *  \brief Test for afl::bits::UInt8
  */

#include "afl/bits/uint8.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.bits.UInt8", a)
{
    uint8_t array[1];

    for (uint_fast16_t i = 0; i < 256; ++i) {
        afl::bits::UInt8::pack(array, static_cast<uint8_t>(i));
        a.checkEqual("value", array[0], i);
        a.checkEqual("unpack", afl::bits::UInt8::unpack(array), i);
    }
}
