/**
  *  \file afl/string/char.hpp
  *  \brief Character Tests
  *
  *  This module is a locale-independant version of <ctype.h>, sort of.
  *  It is intended to support code like parsers, which shall not be locale-dependant.
  *  It also does away with the need to cast everything to <tt>unsigned char</tt>
  *  before calling the function.
  */
#ifndef AFL_AFL_STRING_CHAR_HPP
#define AFL_AFL_STRING_CHAR_HPP

namespace afl { namespace string {

    /** Convert character to upper case (ASCII only).
        \param ch Character
        \return Upcased character */
    char charToUpper(char ch);

    /** Convert character to lower case (ASCII only).
        \param ch Character
        \return Lowercased character */
    char charToLower(char ch);

    /** Check for upper case (ASCII only).
        \param ch Character
        \return true iff ch is an upper-case character */
    bool charIsUpper(char ch);

    /** Check for lower case (ASCII only).
        \param ch Character
        \return true iff ch is a lower-case character */
    bool charIsLower(char ch);

    /** Check for whitespace (ASCII only).
        \param ch Character
        \return true iff ch is a white-space character, i.e. space, newline/CR, tab */
    bool charIsSpace(char ch);

    /** Check for digit (ASCII only).
        \param ch Character
        \return true iff ch is a digit */
    bool charIsDigit(char ch);

    /** Check for alphanumeric (ASCII only).
        \param ch Character
        \return true iff ch is alpahnumeric (upper-case, lower-case, or digit) */
    bool charIsAlphanumeric(char ch);

} }

inline char
afl::string::charToUpper(char ch)
{
    return charIsLower(ch) ? char(ch - 0x20) : ch;
}

inline char
afl::string::charToLower(char ch)
{
    return charIsUpper(ch) ? char(ch + 0x20) : ch;
}

inline bool
afl::string::charIsUpper(char ch)
{
    return (ch >= 'A' && ch <= 'Z');
}

inline bool
afl::string::charIsLower(char ch)
{
    return (ch >= 'a' && ch <= 'z');
}

inline bool
afl::string::charIsSpace(char ch)
{
    return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
}

inline bool
afl::string::charIsDigit(char ch)
{
    return (ch >= '0' && ch <= '9');
}

inline bool
afl::string::charIsAlphanumeric(char ch)
{
    return charIsUpper(ch) || charIsLower(ch) || charIsDigit(ch);
}

#endif
