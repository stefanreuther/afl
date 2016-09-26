/**
  *  \file afl/bits/int8.hpp
  *  \brief Class afl::bits::Int8
  */
#ifndef AFL_AFL_BITS_INT8_HPP
#define AFL_AFL_BITS_INT8_HPP

#include "afl/base/types.hpp"
#include "afl/bits/uint8.hpp"

namespace afl { namespace bits {

    /** Encoding/decoding traits for a int8_t.
        Describes how to convert this type to/from a raw data array. */
    struct Int8 {
        /** Definition of memory for packed data. */
        typedef uint8_t Bytes_t[1];

        /** Data type for unpacked data. */
        typedef int8_t Word_t;

        /** Unpack raw bytes.
            \param bytes raw bytes
            \return unpacked data */
        static Word_t unpack(const Bytes_t& bytes)
            {
                // Conversion from unsigned to signed is unspecified,
                // so be careful to not trigger overflow.
                // gcc optimizes this conditional away entirely.
                uint8_t tmp = UInt8::unpack(bytes);
                if ((tmp & 0x80) != 0) {
                    return -(~tmp) - 1;
                } else {
                    return tmp;
                }
            }

        /** Pack into raw bytes.
            \param bytes [out] raw bytes
            \param word [in] data to pack */
        static void pack(Bytes_t& bytes, Word_t word)
            {
                // Conversion from signed to unsigned is defined as "as-if two's complement",
                // so let the compiler do everything.
                UInt8::pack(bytes, word);
            }
    };

} }

#endif
