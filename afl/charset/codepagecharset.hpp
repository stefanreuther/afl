/**
  *  \file afl/charset/codepagecharset.hpp
  *  \brief Class afl::charset::CodepageCharset
  */
#ifndef AFL_AFL_CHARSET_CODEPAGECHARSET_HPP
#define AFL_AFL_CHARSET_CODEPAGECHARSET_HPP

#include "afl/charset/charset.hpp"

namespace afl { namespace charset {

    struct Codepage;

    /** Codepage Charset implementation.
        Translates to and from UTF-8 using a codepage.
        Characters that cannot be encoded in the codepage are discarded when encoding. */
    class CodepageCharset : public Charset {
     public:
        /** Constructor.
            \param cp Codepage to use for translation. */
        CodepageCharset(const Codepage& cp);

        /** Destructor. */
        virtual ~CodepageCharset();

        // Charset:
        virtual afl::base::GrowableBytes_t encode(afl::string::ConstStringMemory_t in);
        virtual String_t decode(afl::base::ConstBytes_t in);

        // Clonable:
        virtual CodepageCharset* clone() const;

     private:
        const Codepage& m_codepage;
    };

} }


/******************************** Inlines ********************************/

inline afl::charset::CodepageCharset::CodepageCharset(const Codepage& cp)
    : Charset(),
      m_codepage(cp)
{ }

#endif
