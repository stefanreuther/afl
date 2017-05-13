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

        This function determines the boundaries of the original string within \c mem,
        and returns the appropriate subrange.
        Use afl::string::fromBytes() or afl::charset::Charset::decode() to convert it into a real string.

        \param mem [in] data
        \return string bytes */
    afl::base::ConstBytes_t unpackFixedString(afl::base::ConstBytes_t mem);

    /** Pack a fixed-size string.
        The string is copied into the provided target memory,
        which is then padded with space characters.

        This function takes a ConstBytes_t.
        Use afl::string::toBytes() or afl::charset::Charset::encode() to obtain that from a real string.

        \param mem [out] data
        \param src [in] string */
    void packFixedString(afl::base::Bytes_t mem, afl::base::ConstBytes_t src);


    /** Encoding/decoding traits for a fixed-size string.
        Describes how to convert this type to/from a raw data array.

        The string is represented as a ConstBytes_t.
        The idea is to make this fit seamlessly to the afl::charset::Charset interface
        to make it easy to correctly deal with the character set of the serialized string.
        Hence, use afl::charset::Charset methods to convert from and to a real string.
        To just access the bytes of the string, use afl::string::toBytes/afl::string::fromBytes. */
    template<size_t Size>
    struct FixedString {
        /** Definition of memory for packed data. */
        typedef uint8_t Bytes_t[Size];

        /** Data type for unpacked data. */
        typedef afl::base::ConstBytes_t Word_t;

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
                packFixedString(afl::base::Bytes_t(bytes), word);
            }
    };

} }

#endif
