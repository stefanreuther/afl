/**
  *  \file afl/charset/utf8charset.hpp
  *  \brief Class afl::charset::Utf8Charset
  */
#ifndef AFL_AFL_CHARSET_UTF8CHARSET_HPP
#define AFL_AFL_CHARSET_UTF8CHARSET_HPP

#include "afl/charset/charset.hpp"

namespace afl { namespace charset {

    /** UTF-8 Charset implementation.
        This implements the interface of Charset for UTF-8 input.
        Since output is UTF-8 as well, this class effectively does nothing. */
    class Utf8Charset : public Charset {
     public:
        /** Constructor. */
        Utf8Charset();

        /** Destructor. */
        virtual ~Utf8Charset();

        // Charset:
        virtual afl::base::GrowableBytes_t encode(afl::string::ConstStringMemory_t in);
        virtual String_t decode(afl::base::ConstBytes_t in);

        // Clonable:
        virtual Utf8Charset* clone() const;
    };

} }


/******************************** Inlines ********************************/

inline afl::charset::Utf8Charset::Utf8Charset()
{ }

#endif
