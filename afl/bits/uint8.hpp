/**
  *  \file afl/bits/uint8.hpp
  *  \brief Class afl::bits::UInt8
  */
#ifndef AFL_AFL_BITS_UINT8_HPP
#define AFL_AFL_BITS_UINT8_HPP

#include "afl/base/types.hpp"

namespace afl { namespace bits {

    /** Encoding/decoding traits for a uint8_t.
        Describes how to convert this type to/from a raw data array. */
    struct UInt8 {
        /** Definition of memory for packed data. */
        typedef uint8_t Bytes_t[1];

        /** Data type for unpacked data. */
        typedef uint8_t Word_t;

        /** Unpack raw bytes.
            \param bytes raw bytes
            \return unpacked data */
        static Word_t unpack(const Bytes_t& bytes)
            {
                return bytes[0];
            }

        /** Pack into raw bytes.
            \param bytes [out] raw bytes
            \param word [in] data to pack */
        static void pack(Bytes_t& bytes, Word_t word)
            {
                bytes[0] = word;
            }
    };

} }

#endif
