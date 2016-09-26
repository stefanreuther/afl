/**
  *  \file afl/charset/base64.hpp
  *  \brief Class afl::charset::Base64
  */
#ifndef AFL_AFL_CHARSET_BASE64_HPP
#define AFL_AFL_CHARSET_BASE64_HPP

#include "afl/charset/charset.hpp"

namespace afl { namespace charset {

    /** Base-64 Encoding.
        This is not strictly speaking a character set, but fits the interface.
        It encodes an input string (which is not restricted to UTF-8, and can be binary) into Base-64,
        or decodes the same. */
    class Base64 : public Charset {
     public:
        virtual ~Base64();
        virtual String_t encode(afl::string::ConstStringMemory_t in);
        virtual String_t decode(afl::string::ConstStringMemory_t in);
        virtual Base64* clone() const;
    };

} }

#endif
