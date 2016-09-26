/**
  *  \file afl/string/proxytranslator.hpp
  *  \brief Class afl::string::ProxyTranslator
  */
#ifndef AFL_AFL_STRING_PROXYTRANSLATOR_HPP
#define AFL_AFL_STRING_PROXYTRANSLATOR_HPP

#include "afl/string/translator.hpp"

namespace afl { namespace string {

    /** Proxy translator.
        This translator uses another translator to perform translation.
        Use this to break up lifetime issues. */
    class ProxyTranslator : public Translator {
     public:
        /** Constructor.
            \param other Other translator. Must live at least as long as this one. */
        explicit ProxyTranslator(Translator& other);

        // Translator:
        virtual String_t translate(afl::string::ConstStringMemory_t in);

     private:
        Translator& m_other;
    };

} }

#endif
