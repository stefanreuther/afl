/**
  *  \file afl/bits/rotate.hpp
  *  \brief Bit Rotate Functions
  */
#ifndef AFL_AFL_BITS_ROTATE_HPP
#define AFL_AFL_BITS_ROTATE_HPP

#include "afl/base/types.hpp"

namespace afl { namespace bits {

    /** Left-rotate an 8-bit integer.
        \param a Value
        \param c Amount to rotate */
    inline uint8_t
    rotateLeft8(uint8_t a, int c)
    {
        c &= 7;
        return static_cast<uint8_t>((a << c) | (a >> (8-c)));
    }

    /** Right-rotate an 8-bit integer.
        \param a Value
        \param c Amount to rotate */
    inline uint8_t
    rotateRight8(uint8_t a, int c)
    {
        return rotateLeft8(a, 8-c);
    }

    /** Left-rotate a 16-bit integer.
        \param a Value
        \param c Amount to rotate */
    inline uint16_t
    rotateLeft16(uint16_t a, int c)
    {
        c &= 15;
        if (c != 0) {
            return static_cast<uint16_t>((a << c) | (a >> (16-c)));
        } else {
            return a;
        }
    }

    /** Right-rotate a 16-bit integer.
        \param a Value
        \param c Amount to rotate */
    inline uint16_t
    rotateRight16(uint16_t a, int c)
    {
        return rotateLeft16(a, 16-c);
    }

    /** Left-rotate a 32-bit integer.
        \param a Value
        \param c Amount to rotate */
    inline uint32_t
    rotateLeft32(uint32_t a, int c)
    {
        c &= 31;
        if (c != 0) {
            return (a << c) | (a >> (32-c));
        } else {
            return a;
        }
    }

    /** Right-rotate a 32-bit integer.
        \param a Value
        \param c Amount to rotate */
    inline uint32_t
    rotateRight32(uint32_t a, int c)
    {
        c &= 31;
        if (c != 0) {
            return (a << (32-c)) | (a >> c);
        } else {
            return a;
        }
    }

    /** Left-rotate a 64-bit integer.
        \param a Value
        \param c Amount to rotate */
    inline uint64_t
    rotateLeft64(uint64_t a, int c)
    {
        c &= 63;
        if (c != 0) {
            return (a << c) | (a >> (64-c));
        } else {
            return a;
        }
    }

    /** Right-rotate a 64-bit integer.
        \param a Value
        \param c Amount to rotate */
    inline uint64_t
    rotateRight64(uint64_t a, int c)
    {
        c &= 63;
        if (c != 0) {
            return (a << (64-c)) | (a >> c);
        } else {
            return a;
        }
    }

} }

#endif
