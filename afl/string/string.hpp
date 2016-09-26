/**
  *  \file afl/string/string.hpp
  *  \brief String functions
  */
#ifndef AFL_AFL_STRING_STRING_HPP
#define AFL_AFL_STRING_STRING_HPP

#include <string>
#include "afl/base/memory.hpp"
#include "afl/base/types.hpp"

namespace afl { namespace string {

    /*
     *  Types
     */

    /** String type.
        This is the string type used through-out the AFL.
        The idea is to keep a single hook where it can be replaced if needed.
        We assume the full interface provided by the STL string.

        Where it matters, unless otherwise noted, all strings are assumed to be UTF-8. */
    typedef std::string String_t;

    /** Memory type for constant data. */
    typedef afl::base::Memory<const char> ConstStringMemory_t;

    /** Memory type for mutable data. */
    typedef afl::base::Memory<char> StringMemory_t;


    /*
     *  Conversion to/from memory
     */

    /** Convert to Memory.
        Returns a memory object describing the string's characters.
        Does NOT add an implicit null byte.
        Note that the memory only lives as long as the string,
        therefore if the string is a temporary, so is the Memory descriptor.
        \param str String
        \return memory descriptor
        \see toBytes */
    ConstStringMemory_t toMemory(const String_t& str);

    /** Convert null-terminated string (C string) to StringMemory.
        Returns a memory object describing the string's characters,
        NOT including the terminating null byte.
        \param ntbs C string (can be null; will then be treated as empty string)
        \return memory descriptor
        \see toBytes */
    ConstStringMemory_t toMemory(const char* ntbs);

    /** Convert limited null-terminated string (C string) to StringMemory.
        Returns a memory object describing the string's characters,
        NOT including the terminating null byte.
        Returns at most #maxLen bytes.
        \param ntbs C string (can be null; will then be treated as empty string)
        \param maxLen maximum length
        \return memory descriptor */
    ConstStringMemory_t toMemoryLimited(const char* ntbs, size_t maxLen);

    /** Create string from StringMemory.
        Constructs a string by copying the bytes described by %mem.
        \param mem Memory descriptor
        \return string */
    String_t fromMemory(ConstStringMemory_t mem);



    /** Convert null-terminated string (C string) to Bytes.
        Returns a memory object describing the string's characters,
        NOT including the terminating null byte.
        \param str C string (can be null; will then be treated as empty string)
        \return memory descriptor
        \see toMemory */
    afl::base::ConstBytes_t toBytes(const String_t& str);

    /** Convert null-terminated string (C string) to Bytes.
        Returns a memory object describing the string's characters,
        NOT including the terminating null byte.
        \param ntbs C string (can be null; will then be treated as empty string)
        \return memory descriptor
        \see toMemory */
    afl::base::ConstBytes_t toBytes(const char* ntbs);

    /** Create string from Bytes.
        Constructs a string by copying the bytes described by %mem.
        \param mem Memory descriptor
        \return string */
    String_t fromBytes(afl::base::ConstBytes_t mem);



    /*
     *  Case-insensitive comparison (ASCII only)
     */

    /** Compare two strings (base version).
        The comparison ignores differences in ASCII case
        (but does not ignore differences in Unicode characters).
        \param a first string as a byte array
        \param b second string as a byte array
        \return negative if a comes before b, zero if a and b are equivalent, positive if a comes after b. */
    int strCaseCompare(ConstStringMemory_t a, ConstStringMemory_t b);

    /** Compare two strings (C strings).
        The comparison ignores differences in ASCII case
        (but does not ignore differences in Unicode characters).
        \param a first string as a C string (can be null; will then be treated as empty string)
        \param b second string as a C string (can be null; will then be treated as empty string)
        \return negative if a comes before b, zero if a and b are equivalent, positive if a comes after b. */
    int strCaseCompare(const char* a, const char* b);

    /** Compare two strings (C++ strings).
        The comparison ignores differences in ASCII case
        (but does not ignore differences in Unicode characters).
        \param a first string as a C++ string
        \param b second string as a C++ string
        \return negative if a comes before b, zero if a and b are equivalent, positive if a comes after b. */
    int strCaseCompare(const String_t& a, const String_t& b);

    /** Compare two strings (mixed).
        The comparison ignores differences in ASCII case
        (but does not ignore differences in Unicode characters).
        \param a first string as a C string (can be null; will then be treated as empty string)
        \param b second string as a C++ string
        \return negative if a comes before b, zero if a and b are equivalent, positive if a comes after b. */
    int strCaseCompare(const char* a, const String_t& b);

    /** Compare two strings (mixed).
        The comparison ignores differences in ASCII case
        (but does not ignore differences in Unicode characters).
        \param a first string as a C++ string
        \param b second string as a C string (can be null; will then be treated as empty string)
        \return negative if a comes before b, zero if a and b are equivalent, positive if a comes after b. */
    int strCaseCompare(const String_t& a, const char* b);


    /*
     *  Trimming / Parsing
     */

    /** Trim whitespace.
        Removes whitespace from both sides of a string.
        \param s [in] Input
        \return s, with leading and trailing whitespace removed
        \see charIsSpace */
    String_t strTrim(String_t s);

    /** Trim leading (left) whitespace.
        Removes whitespace from the beginning of a string.
        \param s [in] Input
        \return s, with leading whitespace removed
        \see charIsSpace */
    String_t strLTrim(String_t s);

    /** Trim trailing (right) whitespace.
        Removes whitespace from the end of a string.
        \param s [in] Input
        \return s, with trailing whitespace removed
        \see charIsSpace */
    String_t strRTrim(String_t s);

    /** Return leading text up to separator.
        If there is no separator, returns the whole string.
        This can be used for splitting strings into pieces:
        \code
          String_t str = ...
          do {
              process(strFirst(str, ":"));
          } while(strRemove(std, ":"));
        \endcode
        \param str string
        \param sep separator
        \return text before separator */
    String_t strFirst(const String_t str, const String_t sep);

    /** Remove leading text up to separator.
        If the separator is found, removes it and the text before, and returns true.
        If the separator is not found, performs no change and returns false.
        \param str [in/out] string to modify
        \param sep [in] separator
        \return true on success
        \see strFirst */
    bool strRemove(String_t& str, const String_t sep);

    /** Split string at separator.
        If the separator is found, splits up the string into %lhs and %rhs, and returns true.
        Otherwise, returns false.
        \param str [in] string
        \param lhs,rhs [out] place for output
        \param sep [in] separator
        \retval true if separator found, in this case lhs+sep+rhs == str
        \retval false if separator not found, in this case lhs/rhs unmodified */
    bool strSplit(const String_t str, String_t& lhs, String_t& rhs, const String_t sep);


    /*
     *  Case conversion
     */

    /** Convert string to lower case.
        Only ASCII characters are converted.
        \param s string
        \return lower-cased string
        \see charToLower */
    String_t strLCase(String_t s);

    /** Convert string to upper case.
        Only ASCII characters are converted.
        \param s string
        \return upper-cased string
        \see charToUpper */
    String_t strUCase(String_t s);

    /** Convert string to title case.
        The first character of each word will get upcased, all other characters will be lowercased.
        A word is delimited by space or '-'.
        Only ASCII characters are converted.
        \param s string
        \return converted string
        \see charToUpper, charIsSpace */
    String_t strUCFirst(String_t s);

    /** Lower-case ("un-capitalize") words in a string.
        Lower-cases everything but characters that are the first in their word.
        This is similar to #strUCFirst, but it will leave word-starters alone.
        A word is delimited by space or '-'.
        Only ASCII characters are converted.
        \param s string
        \return converted string
        \see charToUpper, charIsSpace */
    String_t strLCWords(String_t s);


    /*
     *  Word extraction
     */

    /** Extract word.
        A word is defined as being surrounded by spaces according to #charIsSpace.
        \param s String
        \param i Word index, zero-based
        \return the word; empty string if %i is too big */
    String_t strNthWord(const String_t& s, size_t i);

    /** Extract word and all following words.
        A word is defined as being surrounded by spaces according to #charIsSpace.
        \param s String
        \param i Word index, zero-based
        \return the word and all following word (space between words remains unchanged); empty string if i is too big */
    String_t strNthWordRest(const String_t& s, size_t i);

} }

/* We export the string type to the public namespace */
using afl::string::String_t;

#endif
