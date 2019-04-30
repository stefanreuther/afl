/**
  *  \file afl/string/languagecode.hpp
  */
#ifndef AFL_AFL_STRING_LANGUAGECODE_HPP
#define AFL_AFL_STRING_LANGUAGECODE_HPP

#include "afl/string/string.hpp"

namespace afl { namespace string {

    /** Language code.
        Represents the identifier of a language or locale.
        The most common form is an identifier of the form "language-location", i.e. "en-GB", "de-DE".
        Variants include a script subtag ("sr-Latn" for Serbian using latin script),
        underscore instead of dash ("en_US"), or a character set ("en_US.UTF-8", "de_DE@euro").
        The identifier starts with the most important token; later tokens become more specific and unimportant.

        This class implements a general and partly ad-hoc parser for these identifiers
        to allow our applications to use language tags provided by the environment.

        References: RfC 4646 */
    class LanguageCode {
     public:
        /** Default constructor.
            Makes an empty language tag.
            \post !isValid() */
        LanguageCode();

        /** Construct from string.
            \param code Language code
            \post get() == code */
        LanguageCode(const String_t& code);

        /** Get language code.
            \return code */
        const String_t& get() const;

        /** Get language code with predefined delimiters.
            Tokens can be separated by various delimiters.
            Use this function to convert a user-specified identifier into one with well-defined delimiters,
            for example, to allow comparisons to fixed identifiers in your app.

            Example: LanguageCode("de_DE@euro").getWithDelimiter("-") = "de-DE-euro"

            \param ch Character */
        String_t getWithDelimiter(char ch) const;

        /** Check validity.
            \return true if language code is valid (non-empty) */
        bool isValid() const;

        /** Compare for equality.
            \param other Other language code
            \return result */
        bool operator==(const LanguageCode& other) const;

        /** Compare for inequality.
            \param other Other language code
            \return result */
        bool operator!=(const LanguageCode& other) const;

        /** Compare for less-than.
            \param other Other language code
            \return result */
        bool operator<(const LanguageCode& other) const;

        /** Compare for less-or-equal.
            \param other Other language code
            \return result */
        bool operator<=(const LanguageCode& other) const;

        /** Compare for greater-than.
            \param other Other language code
            \return result */
        bool operator>(const LanguageCode& other) const;

        /** Compare for greater-or-equal.
            \param other Other language code
            \return result */
        bool operator>=(const LanguageCode& other) const;

        /** Generalize.
            Tries to remove a token from the end of this language code to make it more general.
            For example, "de_DE@euro" becomes "de_DE", then "de".

            In general, a token is a delimiter-separated word.
            Tokens can also end in a token and a single digit ("UTF-8"),
            or start with a single letter token ("x-klingon").

            \retval true if a token could be removed, object has been updated
            \retval false no token can be removed, object unchanged */
        bool generalize();

        /** Match language codes.
            Use this to match a known language code against one given by the user.

            Example: LanguageCode("de").isSameOrMoreGeneral("de_DE")

            Note that this function will match delimiters and case (!LanguageCode("de-DE").isSameOrMoreGeneral("de_DE")),
            so you need to normalize manually (strLCase, getWithDelimiter).

            \param other Other code to match
            \return true on match */
        bool isSameOrMoreGeneral(const LanguageCode& other) const;

     private:
        String_t m_code;
    };

} }

#endif
