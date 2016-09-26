/**
  *  \file u/t_bits_uint8.cpp
  *  \brief Test for afl::bits::UInt8
  */

#include "afl/bits/uint8.hpp"
#include "u/t_bits.hpp"

/** Simple tests. */
void
TestBitsUInt8::testIt()
{
    uint8_t array[1];

    for (uint_fast16_t i = 0; i < 256; ++i) {
        afl::bits::UInt8::pack(array, static_cast<uint8_t>(i));
        TS_ASSERT_EQUALS(array[0], i);
        TS_ASSERT_EQUALS(afl::bits::UInt8::unpack(array), i);
    }
}
