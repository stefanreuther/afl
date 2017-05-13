/**
  *  \file afl/charset/urlencoding.hpp
  *  \brief Class afl::charset::UrlEncoding
  */
#ifndef AFL_AFL_CHARSET_URLENCODING_HPP
#define AFL_AFL_CHARSET_URLENCODING_HPP

#include "afl/charset/charset.hpp"

namespace afl { namespace charset {

    /** URL-Encoding.
        This is not strictly speaking a character set, but fits the interface.
        It encodes an input string (which is not restricted to UTF-8) into URL-encoding ("percent encoding"),
        or decodes the same. */
    class UrlEncoding : public Charset {
     public:
        virtual ~UrlEncoding();
        virtual afl::base::GrowableBytes_t encode(afl::string::ConstStringMemory_t in);
        virtual String_t decode(afl::base::ConstBytes_t in);
        virtual UrlEncoding* clone() const;
    };

} }

#endif
