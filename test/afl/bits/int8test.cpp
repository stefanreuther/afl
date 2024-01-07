/**
  *  \file test/afl/bits/int8test.cpp
  *  \brief Test for afl::bits::Int8
  */

#include "afl/bits/int8.hpp"
#include "afl/test/testrunner.hpp"

/** Simple tests. */
AFL_TEST("afl.bits.Int8", a)
{
    uint8_t array[1];

    for (int_fast16_t i = 0; i < 128; ++i) {
        afl::bits::Int8::pack(array, static_cast<int8_t>(i));
        a.checkEqual("pos value", array[0], i);
        a.checkEqual("pos unpack", afl::bits::Int8::unpack(array), i);
    }

    for (int_fast16_t i = -128; i < 0; ++i) {
        afl::bits::Int8::pack(array, static_cast<int8_t>(i));
        a.checkEqual("neg value", array[0], i + 256);
        a.checkEqual("neg unpack", afl::bits::Int8::unpack(array), i);
    }
}

