/**
  *  \file afl/string/parsememory.hpp
  *  \brief String Parsers for ConstStringMemory_t objects
  *
  *  These functions all
  *  - take a ConstStringMemory_t containing a sequence of characters
  *  - return a number of characters consumed (in particular, 0 if they do not match)
  *  - update the ConstStringMemory_t to remove the consumed characters
  */
#ifndef AFL_AFL_STRING_PARSEMEMORY_HPP
#define AFL_AFL_STRING_PARSEMEMORY_HPP

#include "afl/string/string.hpp"

namespace afl { namespace string {

    /** Parse a sequence of characters matched by a function.
        \param mem [in/out] sequence to parse
        \param pred [in] function (e.g. charIsUpper)
        \return number of characters consumed */
    size_t parseIf(ConstStringMemory_t& mem, bool pred(char));

    /** Parse a single character.
        \param mem [in/out] sequence to parse
        \param ch [in] character to parse
        \return number of characters consumed (0=no match, 1=match) */
    size_t parseChar(ConstStringMemory_t& mem, char ch);

    /** Parse a literal character sequence.
        Expects a sequence of characters that must match completely.
        \param mem [in/out] sequence to parse
        \param lit [in] null-terminated character sequence
        \return number of characters consumed (0=no match, strlen(lit)=match) */
    size_t parseLiteral(ConstStringMemory_t& mem, const char* lit);

    /** Parse a number.
        Expects a positive decimal number.
        \param mem [in/out] sequence to parse
        \param out [out] number
        \return number of characters consumed */
    size_t parseNumber(ConstStringMemory_t& mem, uint64_t& out);

} }

#endif
