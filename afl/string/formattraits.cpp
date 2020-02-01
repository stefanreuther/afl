/**
  *  \file afl/string/formattraits.cpp
  *  \brief Template class afl::string::FormatTraits
  */

#include <climits> // CHAR_BIT
#include <cmath>
#include "afl/string/formattraits.hpp"
#include "afl/string/formatstate.hpp"
#include "afl/string/hex.hpp"

/*
 *  Generic
 */

/* Format string */
String_t
afl::string::formatToString(const String_t& str, FormatState& state)
{
    if (state.getCode() == 'd' || state.getCode() == 'i') {
        state.setNumberConditions(toMemory(str));
    }

    if (state.hasFlag(FormatState::PrecisionFlag) && str.size() > state.getPrecision()) {
        return str.substr(0, state.getPrecision());
    } else {
        return str;
    }
}

/* Format pointer */
String_t
afl::string::formatToString(const void* p, FormatState& state)
{
    if (!p) {
        return "(nil)";
    } else {
        FormatUnion u;
        u.m_unsigned = reinterpret_cast<uintmax_t>(p);
        state.setFlag(FormatState::AlternateFlag);
        state.setCode('x');
        return detail::FormatUnsigned::format(u, state);
    }
}

/* Format pointer as C string or pointer */
String_t
afl::string::formatToString(const char* p, FormatState& state)
{
    if (state.getCode() == 'p') {
        // Pointers render as such
        return formatToString(static_cast<const void*>(p), state);
    } else {
        // Render through memory object
        ConstStringMemory_t result;
        if (p == 0) {
            // Null pointer
            result = toMemory("(nil)");
        } else {
            // String, limited by precision
            if (state.hasFlag(FormatState::PrecisionFlag)) {
                result = toMemoryLimited(p, state.getPrecision());
            } else {
                result = toMemory(p);
            }
        }

        // FIXME: this sets number states according to the truncated text, if any.
        // The cpluslib version used the untruncated text, at the expense of
        // running through the whole string until a null byte. Which is right?
        if (state.getCode() == 'd' || state.getCode() == 'i') {
            state.setNumberConditions(result);
        }
        return fromMemory(result);
    }
}

/*
 *  Float (implemented using generic parameter passing)
 */

namespace {
    const size_t MAX_DIGITS = 100;

    /** Remove trailing zeroes/decimal point from string. */
    void
    trimZeroes(String_t& s)
    {
        String_t::size_type n = s.size();
        while (n > 1 && s[n-1] == '0') {
            --n;
        }
        if (n > 1 && s[n-1] == '.') {
            --n;
        }
        if (n < s.size()) {
            s.erase(n);
        }
    }


    /** Compute v^exp. */
    long double
    ipow(long double v, int exp)
    {
        long double rv = 1;
        if (exp > 0) {
            while (exp) {
                if (exp & 1) {
                    rv *= v;
                    --exp;
                }
                v *= v;
                exp /= 2;
            }
        } else {
            exp = -exp;
            while (exp) {
                if (exp & 1) {
                    rv /= v;
                    --exp;
                }
                v *= v;
                exp /= 2;
            }
        }
        return rv;
    }

    /** Round string so that it contains pre+1 digits.
        \param s     [in/out] String to round. Represents a number in [0,10).
        \param pre   [in] Requested precision; number of fractional digits of s.
        \param expon [in/out] Exponent. Actual value is s*10^expon.
        \param allowExpand [in] Allow increase in precision if rounding requires that.

        If rounding causes the value to become exactly 10.0, this will
        reduce it to 1.0 and increase the exponent in return. If
        allow_expand is true, the precision will be increased as well to
        keep the same number of effective fractional digits for 'f'
        formats. With 'e' formats, precision should not be increased as
        the shift in magnitude will be represented by the 'e+NN' part. */
    void
    roundString(String_t& s, size_t pre, int& expon, bool allowExpand)
    {
        ++pre;
        if (s.length() > pre) {
            String_t::size_type pos = pre;
            if (s[pos] >= '5') {
                while (pos > 0 && s[pos-1] == '9') {
                    s[pos-1] = '0';
                    --pos;
                }
                if (pos == 0) {
                    s.insert(String_t::size_type(0), 1, '1');
                    ++expon;
                    if (allowExpand) {
                        ++pre;
                    }
                } else {
                    s[pos-1]++;
                }
            }
            s.erase(pre);
        }
    }

    /** Returns digits of c, so that c = 'return'*10^'exp_'. */
    String_t
    getDigits(long double c, int& exp_)
    {
        /* figure out exponent */
        int exp2, exp10;
        std::frexp(c, &exp2);
        exp10 = int(exp2 * 19728L / 65536);     // 65536/19728 = 3.3219 = ld 10
        long double divi = ipow(10, exp10);
        if (divi > c && c) {
            divi /= 10;
            --exp10;
        }
        c /= divi;

        /* now, c \in [0,10); format it */
        String_t rv;
        rv.reserve(MAX_DIGITS);
        for (size_t i = 1; i < MAX_DIGITS; ++i) {
            int digit = int(c);
            rv.append(1, char(digit + '0'));
            c = 10*(c - digit);
        }
        exp_ = exp10;
        return rv;
    }

}

/* Format float */
String_t
afl::string::formatToString(const float& val, FormatState& state)
{
    long double ldbl = val;
    return formatToString(ldbl, state);
}

/* Format double */
String_t
afl::string::formatToString(const double& val, FormatState& state)
{
    long double ldbl = val;
    return formatToString(ldbl, state);
}

/* Format long double */
String_t
afl::string::formatToString(const long double& val, FormatState& state)
{
    bool negative = (val < 0);
    long double c = val;
    if (negative) {
        c = -c;
    }

    // special cases for special values
    String_t rv;
    if (std::isnan(c)) {
        // NaN
        rv = "NaN";
    } else if (std::isinf(c)) {
        // Infinity
        rv = "Inf";
    } else {
        // figure out format to use
        bool trim = false;
        size_t pre = state.hasFlag(FormatState::PrecisionFlag) ? state.getPrecision() : 6;
        char typ = state.getCode();
        if (typ == 'g' || typ == 'G') {
            if (pre == 0 || size_t(int(pre)) != pre) {
                pre = 1;
            }
            if (c < 1.0E-4 || c > ipow(10, int(pre))) {
                typ = (typ == 'g' ? 'e' : 'E');
            }
            trim = !state.hasFlag(FormatState::AlternateFlag);
        }

        // figure out exponent
        int expon;
        rv = getDigits(c, expon);

        if (typ == 'e' || typ == 'E') {
            // First, round to requested precision
            roundString(rv, pre, expon, false);
            rv.insert(1, 1, '.');
            if (trim) {
                trimZeroes(rv);
            }

            if (expon || !trim) {
                rv.append(1, typ);

                // Format the exponent
                FormatState fuFlags;
                fuFlags.setCode('d');
                fuFlags.setWidth(3);      // 2 digits + sign
                fuFlags.setFlag(FormatState::SignFlag);
                fuFlags.setFlag(FormatState::ZeroPadFlag);
                if (state.hasFlag(FormatState::GroupFlag)) {
                    fuFlags.setFlag(FormatState::GroupFlag);
                }

                FormatUnion fuExp;
                FormatTraits<int>::store(fuExp, expon);
                rv += FormatTraits<int>::format(fuExp, fuFlags);
            }
        } else {
            // Non-exponential format
            if (expon >= 0) {
                if (rv.length() < pre + size_t(expon)) {
                    rv.append(pre + expon - rv.length(), '0');
                }
                roundString(rv, pre + expon, expon, true);
                if (pre > 0) {
                    rv.insert(expon+1, 1, '.');
                    if (trim) {
                        trimZeroes(rv);
                    }
                }
            } else {
                if (expon < 0) {
                    rv.insert(String_t::size_type(0), -expon, '0');
                    expon = 0;
                    roundString(rv, pre + expon, expon, false);
                } else {
                    roundString(rv, pre + expon, expon, true);
                }
                rv.insert(1, 1, '.');
                if (trim) {
                    trimZeroes(rv);
                }
            }
        }

        // zeropad
        if (state.hasFlag(FormatState::ZeroPadFlag)) {
            size_t n = state.getWidth();
            if (negative || state.hasFlag(FormatState::BlankFlag) || state.hasFlag(FormatState::SignFlag)) {
                if (n > 0) {
                    --n;
                }
            }
            if (n > 0 && rv.length() < n) {
                rv.insert(String_t::size_type(0), n - rv.length(), '0');
            }
        }
    }

    // add sign */
    if (negative) {
        rv.insert(0, "-", 1);
    } else if (state.hasFlag(FormatState::SignFlag)) {
        rv.insert(0, "+", 1);
    } else if (state.hasFlag(FormatState::BlankFlag)) {
        rv.insert(0, " ", 1);
    }

    return rv;
}


/*
 *  Unsigned
 */

namespace {
    /** Maximum size of a formatted unsigned long, and base, with 1000-sep, plus one. Very generous approximation. */
    const size_t DIGIT_BUF_SIZE = (CHAR_BIT * sizeof(uintmax_t) + 1);


    /** Format an unsigned long into a buffer.
        \param c    Value
        \param typ  Format specifier (o/d/u/x/X)
        \param p    Pointer one-past-end of sufficiently large buffer */
    // TODO: ugly
    char*
    formatUnsigned(uintmax_t c, char typ, char* p)
    {
        int base = (typ == 'o') ? 8 : (typ == 'x' || typ == 'X') ? 16 : 10;
        const char*const alpha = (typ == 'X') ? afl::string::HEX_DIGITS_UPPER : afl::string::HEX_DIGITS_LOWER;
        *--p = 0;
        while (c) {
            *--p = alpha[c % base];
            c /= base;
        }
        if (!*p) {
            *--p = '0';
        }
        return p;
    }

    /** Add grouping to a decimal number */
    // TODO: ugly
    void
    addGrouping(String_t& s, String_t::size_type start)
    {
        enum { GROUP = 3 };
        while (start > GROUP) {
            start -= GROUP;
            s.insert(start, ",", 1);
        }
    }
}

/* Format unsigned integer */
String_t
afl::string::detail::FormatUnsigned::format(const FormatUnion& u, FormatState& state)
{
    uintmax_t value = u.m_unsigned;

    state.setCondition(FormatState::ZeroCondition, value == 0);
    state.setCondition(FormatState::OneCondition,  value == 1);

    if (state.getCode() == 'c') {
        return String_t(1, char(value));
    } else {
        char buffer[DIGIT_BUF_SIZE];
        char* p = formatUnsigned(value, state.getCode(), buffer + sizeof buffer);

        /* Now: p -> formatted value */
        String_t value(p);

        /* pre = minimum # of digits */
        if (state.hasFlag(FormatState::PrecisionFlag) && state.getPrecision() > value.length()) {
            value.insert(String_t::size_type(0), state.getPrecision() - value.length(), '0');
        }

        if (state.hasFlag(FormatState::GroupFlag)) {
            addGrouping(value, value.length());
        }

        if (state.hasFlag(FormatState::AlternateFlag)) {
            if (state.getCode() == 'o') {
                if (value[0] != '0') {
                    value.insert(0, "0", 1);
                }
            } else if (state.getCode() == 'x') {
                value.insert(0, "0x", 2);
            } else if (state.getCode() == 'X') {
                value.insert(0, "0X", 2);
            }
        }

        size_t wi = state.getWidth();
        if ((state.hasFlag(FormatState::BlankFlag) || state.hasFlag(FormatState::SignFlag)) && wi > 0) {
            --wi;
        }

        if (value.length() < wi) {
            if (state.hasFlag(FormatState::ZeroPadFlag)) {
                value.insert(String_t::size_type(0), wi - value.length(), '0');
            }
        }
        if (state.hasFlag(FormatState::SignFlag)) {
            value.insert(0, "+", 1);
        } else if (state.hasFlag(FormatState::BlankFlag)) {
            value.insert(0, " ", 1);
        }
        return value;
    }
}

/*
 *  Signed
 */

/* Format signed integer */
String_t
afl::string::detail::FormatSigned::format(const FormatUnion& u, FormatState& state)
{
    if ((state.getCode() != 'd' && state.getCode() != 'i') || u.m_signed >= 0) {
        // Non-negative? Just let the unsigned formatter do it.
        FormatUnion u2;
        u2.m_unsigned = u.m_signed;
        return FormatUnsigned::format(u2, state);
    } else {
        // Signed decimal. Pretty hackish, but should do:
        FormatUnion u2;
        u2.m_unsigned = -u.m_signed;
        state.setFlag(FormatState::SignFlag);

        String_t result = FormatUnsigned::format(u2, state);
        String_t::size_type t = result.find('+');
        if (t != result.size()) {
            result[t] = '-';
        }
        return result;
    }
}
