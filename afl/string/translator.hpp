/**
  *  \file afl/string/translator.hpp
  *  \brief Base class afl::string::Translator
  */
#ifndef AFL_AFL_STRING_TRANSLATOR_HPP
#define AFL_AFL_STRING_TRANSLATOR_HPP

#include <memory>
#include "afl/string/string.hpp"
#include "afl/base/deletable.hpp"
#include "afl/functional/unaryfunction.hpp"

namespace afl { namespace string {

    /** Message translator.
        This is the interface used to allow applications to implement message translation.
        A default implementation exists which does not translate anything. */
    class Translator : public afl::functional::UnaryFunction<String_t, String_t> {
     public:
        /** Translate a string (basic version).
            The string is specified as a ConstStringMemory_t object.
            This method is overridden by base classes.
            \param in String to translate
            \return translated string */
        virtual String_t translate(afl::string::ConstStringMemory_t in) const = 0;

        /** Translate a string (C string convenience version).
            DEPRECATED, use operator().
            This calls translate().
            \param ntbs Null-terminated string
            \return translated string */
        String_t translateString(const char* ntbs) const;

        /** Translate a string (C++ string convenience version).
            DEPRECATED, use operator().
            \param str String
            This calls the translate().
            \return translated string */
        String_t translateString(const String_t& str) const;

        /** UnaryFunction method: get translation. */
        virtual String_t get(String_t a) const;

        /** UnaryFunction override: C string convenience function.
            \param str String */
        String_t operator()(const char* str) const;
        using afl::functional::UnaryFunction<String_t, String_t>::operator();

        /** Get system instance.
            \return a Translator instance to translate system strings. */
        static Translator& getSystemInstance();

        /** Set system instance.
            \param inst Instance to translate system strings. System takes ownership of the object. */
        static void setSystemInstance(std::auto_ptr<Translator> inst);
    };

} }

/** Translate a string.
    All translated strings must use this macro.
    This is equivalent to gettext's "_" macro. */
#define AFL_TRANSLATE_STRING(x) ::afl::string::Translator::getSystemInstance()(x)

/** Mark string for translation.
    This does not actually translate the string, but just keeps the literal as-is.
    This is equivalent to gettext's "N_" macro. */
#define AFL_TRANSLATE_LIT(x) x

#endif
