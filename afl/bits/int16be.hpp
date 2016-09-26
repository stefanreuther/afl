/**
  *  \file afl/bits/int16be.hpp
  *  \brief Class afl::bits::Int16BE
  */
#ifndef AFL_AFL_BITS_INT16BE_HPP
#define AFL_AFL_BITS_INT16BE_HPP

#include "afl/base/types.hpp"
#include "afl/bits/uint16be.hpp"

namespace afl { namespace bits {

    /** Encoding/decoding traits for a int16_t, big endian.
        Describes how to convert this type to/from a raw data array. */
    struct Int16BE {
        /** Definition of memory for packed data. */
        typedef uint8_t Bytes_t[2];

        /** Data type for unpacked data. */
        typedef int16_t Word_t;

        /** Unpack raw bytes.
            \param bytes raw bytes
            \return unpacked data */
        static Word_t unpack(const Bytes_t& bytes)
            {
                // Conversion from unsigned to signed is unspecified,
                // so be careful to not trigger overflow.
                // gcc optimizes this conditional away entirely.
                uint16_t tmp = UInt16BE::unpack(bytes);
                if ((tmp & 0x8000) != 0) {
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
                UInt16BE::pack(bytes, word);
            }
    };

} }

#endif
