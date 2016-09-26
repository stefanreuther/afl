/**
  *  \file afl/bits/bits.hpp
  *  \brief Bitwise Operations
  */
#ifndef AFL_AFL_BITS_BITS_HPP
#define AFL_AFL_BITS_BITS_HPP

#include "afl/base/types.hpp"

namespace afl { namespace bits {

    /** Check for power of two.
        Templatized to avoid unneeded widening of the operands.
        \param T value type
        \param i value
        \return true iff i is a power of two. 0 is considered a power of two by this function. */
    template<typename T>
    bool isPowerOfTwo(T i) throw();

    /** Bitwise logarithm, 32 bits.
        Locates the most-significant set bit in the value %v, and returns its position.
        This is an approximation of the base-2 logarithm.
        \param v value to test
        \return bit position p such that (v>>p) == 1; -1 if v is 0 and no such position exists. */
    int bitLog32(uint32_t v) throw();

    /** Bitwise logarithm, 64 bits.
        Locates the most-significant set bit in the value %v, and returns its position.
        This is an approximation of the base-2 logarithm.
        \param v value to test
        \return bit position p such that (v>>p) == 1; -1 if v is 0 and no such position exists. */
    int bitLog64(uint64_t v) throw();

    /** Reverse bits, 8 bits.
        The least-significant bit becomes the most significant, and vice versa.
        \param v value
        \return reversed value */
    uint8_t  bitReverse8(uint8_t v) throw();

    /** Reverse bits, 16 bits.
        The least-significant bit becomes the most significant, and vice versa.
        \param v value
        \return reversed value */
    uint16_t bitReverse16(uint16_t v) throw();

    /** Reverse bits, 32 bits.
        The least-significant bit becomes the most significant, and vice versa.
        \param v value
        \return reversed value */
    uint32_t bitReverse32(uint32_t v) throw();

    /** Reverse bits, 64 bits.
        The least-significant bit becomes the most significant, and vice versa.
        \param v value
        \return reversed value */
    uint64_t bitReverse64(uint64_t v) throw();

} }


template<typename T>
inline bool
afl::bits::isPowerOfTwo(T i) throw()
{
    return (i & (i-1)) == 0;
}


#endif
