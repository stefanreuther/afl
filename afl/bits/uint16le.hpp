/**
  *  \file afl/bits/uint16le.hpp
  *  \brief Class afl::bits::UInt16LE
  */
#ifndef AFL_AFL_BITS_UINT16LE_HPP
#define AFL_AFL_BITS_UINT16LE_HPP

#include "afl/base/types.hpp"

namespace afl { namespace bits {

    /** Encoding/decoding traits for a uint16_t, little endian.
        Describes how to convert this type to/from a raw data array. */
    struct UInt16LE {
        /** Definition of memory for packed data. */
        typedef uint8_t Bytes_t[2];

        /** Data type for unpacked data. */
        typedef uint16_t Word_t;

        /** Unpack raw bytes.
            \param bytes raw bytes
            \return unpacked data */
        static Word_t unpack(const Bytes_t& bytes)
            {
                return static_cast<Word_t>(256*bytes[1] + bytes[0]);
            }

        /** Pack into raw bytes.
            \param bytes [out] raw bytes
            \param word [in] data to pack */
        static void pack(Bytes_t& bytes, Word_t word)
            {
                bytes[0] = static_cast<uint8_t>(word & 255);
                bytes[1] = static_cast<uint8_t>((word >> 8) & 255);
            }
    };

} }

#endif
