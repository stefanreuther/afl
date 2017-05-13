/**
  *  \file afl/charset/hexencoding.hpp
  *  \brief Class afl::charset::HexEncoding
  */
#ifndef AFL_AFL_CHARSET_HEXENCODING_HPP
#define AFL_AFL_CHARSET_HEXENCODING_HPP

#include "afl/charset/charset.hpp"

namespace afl { namespace charset {

    /** Hex Encoding.
        This is not strictly speaking a character set, but fits the interface.
        It encodes an input string (which is not restricted to UTF-8) into hex encoding (two hex digits per byte),
        or decodes the same. */
    class HexEncoding : public Charset {
     public:
        HexEncoding();
        HexEncoding(const char (&digits)[16]);
        virtual ~HexEncoding();
        virtual afl::base::GrowableBytes_t encode(afl::string::ConstStringMemory_t in);
        virtual String_t decode(afl::base::ConstBytes_t in);
        virtual HexEncoding* clone() const;

     private:
        const char (&m_digits)[16];
    };

} }

#endif
