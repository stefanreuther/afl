/**
  *  \file afl/charset/quotedprintable.hpp
  *  \brief Class afl::charset::QuotedPrintable
  */
#ifndef AFL_AFL_CHARSET_QUOTEDPRINTABLE_HPP
#define AFL_AFL_CHARSET_QUOTEDPRINTABLE_HPP

#include "afl/charset/charset.hpp"

namespace afl { namespace charset {

    /** Quoted-Printable Character Encoding.
        This is not strictly speaking a character set, but fits the interface.
        It encodes an input string (which is not restricted to UTF-8) into quoted-printable encoding,
        or decodes the same. */
    class QuotedPrintable : public Charset {
     public:
        virtual ~QuotedPrintable();
        virtual String_t encode(afl::string::ConstStringMemory_t in);
        virtual String_t decode(afl::string::ConstStringMemory_t in);
        virtual QuotedPrintable* clone() const;
    };

} }

#endif
