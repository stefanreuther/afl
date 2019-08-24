/**
  *  \file afl/charset/asciitransliterator.hpp
  *  \brief Class afl::charset::AsciiTransliterator
  */
#ifndef AFL_AFL_CHARSET_ASCIITRANSLITERATOR_HPP
#define AFL_AFL_CHARSET_ASCIITRANSLITERATOR_HPP

#include "afl/charset/charset.hpp"

namespace afl { namespace charset {

    /** Unicode-to-ASCII transliteration.
        This Charset accepts input in Unicode (UTF-8) and translates it into a simple ASCII representation.
        The transliteration is intentionally simple and does not try to match any standardized language transliteration.

        - decode(): input is UTF-8, output is transliterated ASCII
        - encode(): input is returned without modification, transliteration is not reversible

        This transliteration supports a large subset of Unicode, covering WGL4 and then some.
        Far-east scripts are currently out-of-scope.

        This is an implementation of the Unidecode transliterator, see <https://metacpan.org/pod/Text::Unidecode>. */
    class AsciiTransliterator : public Charset {
     public:
        /** Constructor. */
        AsciiTransliterator();

        /** Destructor. */
        virtual ~AsciiTransliterator();

        // Charset:
        virtual afl::base::GrowableBytes_t encode(afl::string::ConstStringMemory_t in);
        virtual String_t decode(afl::base::ConstBytes_t in);

        // Clonable:
        virtual AsciiTransliterator* clone() const;
    };

} }

inline
afl::charset::AsciiTransliterator::AsciiTransliterator()
{ }

#endif
