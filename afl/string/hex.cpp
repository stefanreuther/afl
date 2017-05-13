/**
  *  \file afl/string/hex.cpp
  *  \brief Hex Digits
  */

#include "afl/string/hex.hpp"

const char afl::string::HEX_DIGITS_UPPER[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
const char afl::string::HEX_DIGITS_LOWER[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

int
afl::string::getHexDigitValue(char c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    } else {
        return -1;
    }
}

void
afl::string::putHexByte(String_t& s, uint8_t b)
{
    putHexByte(s, b, HEX_DIGITS_UPPER);
}

void
afl::string::putHexByte(String_t& s, uint8_t b, const char (&digits)[16])
{
    s += digits[(b >> 4) & 15];
    s += digits[b & 15];
}
