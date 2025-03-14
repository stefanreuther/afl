/**
  *  \file afl/bits/int32le.hpp
  *  \brief Class afl::bits::Int32LE
  */
#ifndef AFL_AFL_BITS_INT32LE_HPP
#define AFL_AFL_BITS_INT32LE_HPP

#include "afl/base/types.hpp"
#include "afl/bits/uint32le.hpp"

namespace afl { namespace bits {

    /** Encoding/decoding traits for a int32_t, little endian.
        Describes how to convert this type to/from a raw data array. */
    struct Int32LE {
        /** Definition of memory for packed data. */
        typedef uint8_t Bytes_t[4];

        /** Data type for unpacked data. */
        typedef int32_t Word_t;

        /** Unpack raw bytes.
            \param bytes raw bytes
            \return unpacked data */
        static Word_t unpack(const Bytes_t& bytes)
            {
                // Conversion from unsigned to signed is unspecified,
                // so be careful to not trigger overflow.
                // gcc optimizes this conditional away entirely.
                uint32_t tmp = UInt32LE::unpack(bytes);
                if ((tmp & 0x80000000UL) != 0) {
                    return static_cast<int32_t>(-(~tmp) - 1);
                } else {
                    return static_cast<int32_t>(tmp);
                }
            }

        /** Pack into raw bytes.
            \param bytes [out] raw bytes
            \param word [in] data to pack */
        static void pack(Bytes_t& bytes, Word_t word)
            {
                // Conversion from signed to unsigned is defined as "as-if two's complement",
                // so let the compiler do everything.
                UInt32LE::pack(bytes, static_cast<uint32_t>(word));
            }
    };

} }

#endif
