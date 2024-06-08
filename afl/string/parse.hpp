/**
  *  \file afl/string/parse.hpp
  *  \brief String Parsers
  */
#ifndef AFL_AFL_STRING_PARSE_HPP
#define AFL_AFL_STRING_PARSE_HPP

#include "afl/base/types.hpp"
#include "afl/string/string.hpp"
#include "afl/base/staticassert.hpp"

namespace afl { namespace string {

    /** Parse decimal string into integer (strtol/strtoul).
        \tparam T Numeric type (signed or unsigned integer, up to long/int32_t)
        \param str   [in] String to parse
        \param value [out] Value; modified only if string is completely valid
        \param pos   [out] If string is not valid, number of characters that form a possibly-valid prefix
        \retval true String was completely valid, \c value has been set.
        \retval false String was not completely valid, \c pos has been set. */
    template<typename T>
    bool strToInteger(const String_t& str, T& value, String_t::size_type& pos);
    bool strToInteger(const String_t& str, long& value, String_t::size_type& pos);
    bool strToInteger(const String_t& str, unsigned long& value, String_t::size_type& pos);
    bool strToInteger(const String_t& str, long long& value, String_t::size_type& pos);
    bool strToInteger(const String_t& str, unsigned long long& value, String_t::size_type& pos);

    /** Parse decimal string into integer (strtol/strtoul).
        This differs from the other signature in that it does not return the position.
        \tparam T Numeric type (signed or unsigned integer, up to long/int32_t)
        \param str   [in] String to parse
        \param value [out] Value; modified only if string is completely valid
        \retval true String was completely valid, \c value has been set.
        \retval false String was not completely valid. */
    template<typename T>
    bool strToInteger(const String_t& str, T& value);

    /** Parse decimal string into floating-point number (strtod).
        \param str   [in] String to parse
        \param value [out] Value; modified only if string is completely valid
        \param pos   [out] If string is not valid, number of characters that form a possibly-valid prefix
        \retval true String was completely valid, \c value has been set.
        \retval false String was not completely valid, \c pos has been set. */
    bool strToFloat(const String_t& str, double& value, String_t::size_type& pos);

    /** Parse decimal string into floating-point number (strtod).
        This differs from the other signature in that it does not return the position.
        \param str   [in] String to parse
        \param value [out] Value; modified only if string is completely valid
        \retval true String was completely valid, \c value has been set.
        \retval false String was not completely valid. */
    bool strToFloat(const String_t& str, double& value);
} }

/* Template implementation that dispatches into the two direct implementations */
template<typename T>
bool
afl::string::strToInteger(const String_t& str, T& value, String_t::size_type& pos)
{
    static_assert(sizeof(T) <= sizeof(unsigned long), "cannot parse anything longer than long");
    if (T(-1) > 0) {
        // Unsigned
        unsigned long tmp;
        bool result = strToInteger(str, tmp, pos);
        if (result) {
            if (static_cast<unsigned long>(T(tmp)) != tmp) {
                pos = 0;
                result = false;
            } else {
                value = T(tmp);
            }
        }
        return result;
    } else {
        // Signed
        long tmp;
        bool result = strToInteger(str, tmp, pos);
        if (result) {
            if (static_cast<long>(T(tmp)) != tmp) {
                pos = 0;
                result = false;
            } else {
                value = T(tmp);
            }
        }
        return result;
    }
}

template<typename T>
bool
afl::string::strToInteger(const String_t& str, T& value)
{
    String_t::size_type pos;
    return strToInteger(str, value, pos);
}

#endif
