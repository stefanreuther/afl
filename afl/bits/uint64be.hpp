/**
  *  \file afl/bits/uint64be.hpp
  *  \brief Class afl::bits::UInt64BE
  */
#ifndef AFL_AFL_BITS_UINT64BE_HPP
#define AFL_AFL_BITS_UINT64BE_HPP

#include "afl/base/types.hpp"

namespace afl { namespace bits {

    /** Encoding/decoding traits for a uint64_t, big endian.
        Describes how to convert this type to/from a raw data array. */
    struct UInt64BE {
        /** Definition of memory for packed data. */
        typedef uint8_t Bytes_t[8];

        /** Data type for unpacked data. */
        typedef uint64_t Word_t;

        /** Unpack raw bytes.
            \param bytes raw bytes
            \return unpacked data */
        static Word_t unpack(const Bytes_t& bytes)
            {
                return static_cast<uint64_t>(bytes[7])
                    + (static_cast<uint64_t>(bytes[6]) << 8)
                    + (static_cast<uint64_t>(bytes[5]) << 16)
                    + (static_cast<uint64_t>(bytes[4]) << 24)
                    + (static_cast<uint64_t>(bytes[3]) << 32)
                    + (static_cast<uint64_t>(bytes[2]) << 40)
                    + (static_cast<uint64_t>(bytes[1]) << 48)
                    + (static_cast<uint64_t>(bytes[0]) << 56);
            }

        /** Pack into raw bytes.
            \param bytes [out] raw bytes
            \param word [in] data to pack */
        static void pack(Bytes_t& bytes, Word_t word)
            {
                bytes[0] = static_cast<uint8_t>((word >> 56) & 255);
                bytes[1] = static_cast<uint8_t>((word >> 48) & 255);
                bytes[2] = static_cast<uint8_t>((word >> 40) & 255);
                bytes[3] = static_cast<uint8_t>((word >> 32) & 255);
                bytes[4] = static_cast<uint8_t>((word >> 24) & 255);
                bytes[5] = static_cast<uint8_t>((word >> 16) & 255);
                bytes[6] = static_cast<uint8_t>((word >> 8) & 255);
                bytes[7] = static_cast<uint8_t>(word & 255);
            }
    };

} }

#endif
