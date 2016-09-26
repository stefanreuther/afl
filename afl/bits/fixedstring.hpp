/**
  *  \file afl/bits/fixedstring.hpp
  *  \brief Class afl::bits::FixedString
  */
#ifndef AFL_AFL_BITS_FIXEDSTRING_HPP
#define AFL_AFL_BITS_FIXEDSTRING_HPP

#include "afl/base/types.hpp"
#include "afl/string/string.hpp"

namespace afl { namespace bits {

    /** Unpack a fixed-size string.
        The fixed-size string consists of a sequence of characters,
        padded to a fixed field width using space characters.
        Alternatively, the string can be terminated by a null character.
        \param mem [in] data
        \return string */
    String_t unpackFixedString(afl::base::ConstBytes_t mem);

    /** Pack a fixed-size string.
        The string is copied into the provided target memory,
        which is then padded with space characters.
        \param mem [out] data
        \param src [in] string */
    void packFixedString(afl::base::Bytes_t mem, afl::string::ConstStringMemory_t src);


    /** Encoding/decoding traits for a fixed-size string.
        Describes how to convert this type to/from a raw data array.
        The string is returned as an actual C++ string. */
    template<size_t Size>
    struct FixedString {
        /** Definition of memory for packed data. */
        typedef uint8_t Bytes_t[Size];

        /** Data type for unpacked data. */
        typedef String_t Word_t;

        /** Unpack raw bytes.
            \param bytes raw bytes
            \return unpacked data */
        static Word_t unpack(const Bytes_t& bytes)
            {
                return unpackFixedString(afl::base::ConstBytes_t(bytes));
            }

        /** Pack into raw bytes.
            \param bytes [out] raw bytes
            \param word [in] data to pack */
        static void pack(Bytes_t& bytes, const Word_t& word)
            {
                packFixedString(afl::base::Bytes_t(bytes), afl::string::toMemory(word));
            }
    };

} }

#endif
