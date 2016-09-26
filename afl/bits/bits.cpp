/**
  *  \file afl/bits/bits.cpp
  *  \brief Bitwise Operations
  */

#include "afl/bits/bits.hpp"

int
afl::bits::bitLog32(uint32_t v) throw()
{
    if (v == 0) {
        return -1;
    } else {
        int result = 0;
        for (int bitNr = 16; bitNr != 0; bitNr >>= 1) {
            if ((v >> bitNr) != 0) {
                v >>= bitNr;
                result += bitNr;
            }
        }
        return result;
    }
}

int
afl::bits::bitLog64(uint64_t v) throw()
{
    if (v == 0) {
        return -1;
    } else {
        int result = 0;
        for (int bitNr = 32; bitNr != 0; bitNr >>= 1) {
            if ((v >> bitNr) != 0) {
                v >>= bitNr;
                result += bitNr;
            }
        }
        return result;
    }
}

uint8_t
afl::bits::bitReverse8(uint8_t a) throw()
{
    static const uint8_t table[] = {
        0,  8, 4, 12,
        2, 10, 6, 14,
        1,  9, 5, 13,
        3, 11, 7, 15
    };
    return static_cast<uint8_t>((table[a & 15] << 4) | (table[(a >> 4) & 15]));
}

uint16_t
afl::bits::bitReverse16(uint16_t a) throw()
{
    return uint16_t(bitReverse8(uint8_t(a >> 8))
                    | (uint16_t(bitReverse8(uint8_t(a & 255))) << 8));
}

uint32_t
afl::bits::bitReverse32(uint32_t a) throw()
{
    return uint32_t(bitReverse16(uint16_t(a >> 16))
                    | (uint32_t(bitReverse16(uint16_t(a & 65535))) << 16));
}

uint64_t
afl::bits::bitReverse64(uint64_t a) throw()
{
    return uint64_t(bitReverse32(uint32_t(a >> 32))
                    | (uint64_t(bitReverse32(uint32_t(a & 0xFFFFFFFF))) << 32));
}
