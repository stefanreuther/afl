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

/*
 *  Bit Population Count:
 *
 *  The "& 0x55" versions have constant time, whereas the "v &= (v-1)" version generates shorter code.
 *  For now, select the shorter code.
 */

size_t
afl::bits::bitPop8(uint8_t v) throw()
{
#if 0
    v = (v & 0x55) + ((v & 0xAA) >> 1);
    v = (v & 0x33) + ((v & 0xCC) >> 2);
    v = (v & 0x0F) + ((v & 0xF0) >> 4);
    return v;
#else
    return bitPop32(v);
#endif
}

size_t
afl::bits::bitPop16(uint16_t v) throw()
{
#if 0
    v = (v & 0x5555) + ((v & 0xAAAA) >> 1);
    v = (v & 0x3333) + ((v & 0xCCCC) >> 2);
    v = (v & 0x0F0F) + ((v & 0xF0F0) >> 4);
    v = (v & 0x00FF) + ((v & 0xFF00) >> 8);
    return v;
#else
    return bitPop32(v);
#endif
}

size_t
afl::bits::bitPop32(uint32_t v) throw()
{
#if 0
    v = (v & 0x55555555) + ((v & 0xAAAAAAAA) >> 1);
    v = (v & 0x33333333) + ((v & 0xCCCCCCCC) >> 2);
    v = (v & 0x0F0F0F0F) + ((v & 0xF0F0F0F0) >> 4);
    v = (v & 0x00FF00FF) + ((v & 0xFF00FF00) >> 8);
    v = (v & 0x0000FFFF) + ((v & 0xFFFF0000) >> 16);
    return v;
#else
    size_t result = 0;
    while (v) {
        v &= (v-1);
        ++result;
    }
    return result;
#endif
}

size_t
afl::bits::bitPop64(uint64_t v) throw()
{
#if 0
    v = (v & 0x5555555555555555) + ((v & 0xAAAAAAAAAAAAAAAA) >> 1);
    v = (v & 0x3333333333333333) + ((v & 0xCCCCCCCCCCCCCCCC) >> 2);
    v = (v & 0x0F0F0F0F0F0F0F0F) + ((v & 0xF0F0F0F0F0F0F0F0) >> 4);
    v = (v & 0x00FF00FF00FF00FF) + ((v & 0xFF00FF00FF00FF00) >> 8);
    v = (v & 0x0000FFFF0000FFFF) + ((v & 0xFFFF0000FFFF0000) >> 16);
    v = (v & 0x00000000FFFFFFFF) + ((v & 0xFFFFFFFF00000000) >> 32);
    return v;
#else
    size_t result = 0;
    while (v) {
        v &= (v-1);
        ++result;
    }
    return result;
#endif
}
