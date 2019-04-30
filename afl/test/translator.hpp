/**
  *  \file afl/test/translator.hpp
  *  \brief Class afl::test::Translator
  */
#ifndef AFL_AFL_TEST_TRANSLATOR_HPP
#define AFL_AFL_TEST_TRANSLATOR_HPP

#include "afl/string/translator.hpp"

namespace afl { namespace test {

    /** Generic Translator mock.
        Implements translation by surrounding the strings to translate using open/close strings.
        This makes it obvious in tests when a string went through translation or didn't. */
    class Translator : public afl::string::Translator {
     public:
        /** Default constructor.
            This translator will surround strings in "<" and ">". */
        Translator();

        /** Constructor.
            \param open String to prepend to translations
            \param close String to append to translations */
        Translator(String_t open, String_t close);

        /** Destructor. */
        ~Translator();

        // Translator:
        virtual String_t translate(afl::string::ConstStringMemory_t in) const;

     private:
        String_t m_open;
        String_t m_close;
    };

} }

#endif
