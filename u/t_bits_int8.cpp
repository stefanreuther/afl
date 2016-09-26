/**
  *  \file u/t_bits_int8.cpp
  *  \brief Test for afl::bits::Int8
  */

#include "afl/bits/int8.hpp"
#include "u/t_bits.hpp"

/** Simple tests. */
void
TestBitsInt8::testIt()
{
    uint8_t array[1];

    for (int_fast16_t i = 0; i < 128; ++i) {
        afl::bits::Int8::pack(array, static_cast<int8_t>(i));
        TS_ASSERT_EQUALS(array[0], i);
        TS_ASSERT_EQUALS(afl::bits::Int8::unpack(array), i);
    }

    for (int_fast16_t i = -128; i < 0; ++i) {
        afl::bits::Int8::pack(array, static_cast<int8_t>(i));
        TS_ASSERT_EQUALS(array[0], i + 256);
        TS_ASSERT_EQUALS(afl::bits::Int8::unpack(array), i);
    }
}

