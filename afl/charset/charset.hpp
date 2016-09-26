/**
  *  \file afl/charset/charset.hpp
  *  \brief Base class afl::charset::Charset
  */
#ifndef AFL_AFL_CHARSET_CHARSET_HPP
#define AFL_AFL_CHARSET_CHARSET_HPP

#include "afl/base/deletable.hpp"
#include "afl/string/string.hpp"
#include "afl/base/clonable.hpp"

namespace afl { namespace charset {

    /** Character set encoder/decoder.
        This is the base class for logic for encoding UTF-8 into a specific character set
        used on an external interface, or decoding it back. */
    class Charset : public afl::base::Deletable,
                    public afl::base::Clonable<Charset>
    {
     public:
        /** Encode UTF-8 string into this charset.
            \param in Input string, UTF-8
            \return Result, in class-specific charset */
        virtual String_t encode(afl::string::ConstStringMemory_t in) = 0;

        /** Decode charset string into UTF-8.
            \param in Input string, in class-specific charset
            \return Result, UTF-8 */
        virtual String_t decode(afl::string::ConstStringMemory_t in) = 0;
    };

} }

#endif
