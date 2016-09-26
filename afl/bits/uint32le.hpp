/**
  *  \file afl/bits/uint32le.hpp
  *  \brief Class afl::bits::UInt32LE
  */
#ifndef AFL_AFL_BITS_UINT32LE_HPP
#define AFL_AFL_BITS_UINT32LE_HPP

#include "afl/base/types.hpp"

namespace afl { namespace bits {

    /** Encoding/decoding traits for a uint32_t, little endian.
        Describes how to convert this type to/from a raw data array. */
    struct UInt32LE {
        /** Definition of memory for packed data. */
        typedef uint8_t Bytes_t[4];

        /** Data type for unpacked data. */
        typedef uint32_t Word_t;

        /** Unpack raw bytes.
            \param bytes raw bytes
            \return unpacked data */
        static Word_t unpack(const Bytes_t& bytes)
            {
                return static_cast<uint32_t>(bytes[0])
                    + (static_cast<uint32_t>(bytes[1]) << 8)
                    + (static_cast<uint32_t>(bytes[2]) << 16)
                    + (static_cast<uint32_t>(bytes[3]) << 24);
            }

        /** Pack into raw bytes.
            \param bytes [out] raw bytes
            \param word [in] data to pack */
        static void pack(Bytes_t& bytes, Word_t word)
            {
                bytes[0] = static_cast<uint8_t>(word & 255);
                bytes[1] = static_cast<uint8_t>((word >> 8) & 255);
                bytes[2] = static_cast<uint8_t>((word >> 16) & 255);
                bytes[3] = static_cast<uint8_t>((word >> 24) & 255);
            }
    };

} }

#endif
