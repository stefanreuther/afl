/**
  *  \file afl/string/hex.hpp
  *  \brief Hex Digits
  */
#ifndef AFL_AFL_STRING_HEX_HPP
#define AFL_AFL_STRING_HEX_HPP

#include "afl/string/string.hpp"
#include "afl/base/types.hpp"
#include "afl/base/growablememory.hpp"

namespace afl { namespace string {

    /** Hex digits, upper-case. */
    extern const char HEX_DIGITS_UPPER[16];

    /** Hex digits, lower-case. */
    extern const char HEX_DIGITS_LOWER[16];

    /** Get value of a character as hex digit.
        \param c Character
        \return Hex digit value [0,15] for valid characters, -1 for invalid characters */
    int getHexDigitValue(char c);

    /** Format byte as two hex digits.
        Uses upper-case digits.
        \param s [in/out] Result will be appended here
        \param b Hex byte */
    void putHexByte(String_t& s, uint8_t b);

    /** Format byte as two hex digits.
        Uses specified digits.
        \param s [in/out] Result will be appended here
        \param b Hex byte
        \param digits Digit characters to use */
    void putHexByte(String_t& s, uint8_t b, const char (&digits)[16]);

} }

#endif
