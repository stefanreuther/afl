/**
  *  \file afl/charset/unicode.hpp
  *  \brief Unicode Definitions
  */
#ifndef AFL_AFL_CHARSET_UNICODE_HPP
#define AFL_AFL_CHARSET_UNICODE_HPP

#include "afl/base/types.hpp"

namespace afl { namespace charset {

    /** Unicode character.
        We cannot use wchar_t because that cannot hold a Unicode character everywhere. */
    typedef uint32_t Unichar_t;

    /*
     *  Unicode Range
     */

    /** Maximum allowed Unicode character. */
    const Unichar_t UNICODE_MAX = 0x10FFFF;


    /*
     *  Surrogates
     */

    /** Lower bound of "high surrogate" range.
        The high surrogate appears first and provides the high 10 bits of a codepoint. */
    const Unichar_t UNICODE_HIGH_SURROGATE_MIN = 0xD800;

    /** Upper bound of "high surrogate" range. */
    const Unichar_t UNICODE_HIGH_SURROGATE_MAX = 0xDBFF;

    /** Lower bound of "low surrogate" range.
        The low surrogate appears second and provides the low 10 bits of a codepoint. */
    const Unichar_t UNICODE_LOW_SURROGATE_MIN = 0xDC00;

    /** Upper bound of "low surrogate" range. */
    const Unichar_t UNICODE_LOW_SURROGATE_MAX = 0xDFFF;



    /*
     *  Character Properties
     */

    /** Convert character to upper case.
        This function supports a large subset of Unicode, covering WGL4 and then some.
        \param ch Input character
        \return Upper-case version of character */
    Unichar_t getUpperCase(Unichar_t ch);

    /** Convert character to lower case.
        This function supports a large subset of Unicode, covering WGL4 and then some.
        \param ch Input character
        \return Lower-case version of character */
    Unichar_t getLowerCase(Unichar_t ch);


    /*
     *  Error Characters
     *
     *  The "error character" range is used to provide roundtrip transcoding transparency.
     *
     *  We use the range 0xEF80..0xEFFF reserved for "encoding hacks" in CSUR.
     *  PCC2 used 0xE000, which is reserved for Tengwar.
     */

    /** Lower bound of "error character" range. */
    const Unichar_t UNICODE_ERROR_MIN = 0xEF80;

    /** Upper bound of "error character" range. */
    const Unichar_t UNICODE_ERROR_MAX = 0xEFFF;

    /** Make an error character.
        \param id Error character Id (0x80 .. 0xFF)
        \return Error character */
    Unichar_t makeErrorCharacter(uint8_t id);

    /** Check for error character.
        \param uc Unicode character to check
        \return true iff uc is an error character */
    bool isErrorCharacter(Unichar_t uc);

    /** Get Id of error character.
        \param uc Unicode character
        \pre isErrorCharacter(uc)
        \return error character Id (0x80 .. 0xFF) */
    uint8_t getErrorCharacterId(Unichar_t uc);

} }


inline afl::charset::Unichar_t
afl::charset::makeErrorCharacter(uint8_t id)
{
    return UNICODE_ERROR_MIN + (id - 0x80U);
}

inline bool
afl::charset::isErrorCharacter(Unichar_t uc)
{
    return uc >= UNICODE_ERROR_MIN
        && uc <= UNICODE_ERROR_MAX;
}

inline uint8_t
afl::charset::getErrorCharacterId(Unichar_t uc)
{
    return static_cast<uint8_t>(uc - UNICODE_ERROR_MIN + 0x80);
}


#endif
