/**
  *  \file afl/string/nulltranslator.hpp
  *  \brief Class afl::string::NullTranslator
  */
#ifndef AFL_AFL_STRING_NULLTRANSLATOR_HPP
#define AFL_AFL_STRING_NULLTRANSLATOR_HPP

#include "afl/string/translator.hpp"

namespace afl { namespace string {

    /** Null translator.
        This is a translator that doesn't actually do anything but return the string it was invoked with. */
    class NullTranslator : public Translator {
     public:
        /** Constructor. */
        NullTranslator();

        /** Virtual destructor. */
        virtual ~NullTranslator();

        virtual String_t translate(afl::string::ConstStringMemory_t in) const;
    };

} }

#endif
