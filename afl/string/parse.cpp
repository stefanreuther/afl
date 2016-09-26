/**
  *  \file afl/string/parse.cpp
  *  \brief String Parsers
  */

#include <cstdlib>
#include <cerrno>
#include <climits>
#include "afl/string/parse.hpp"
#include "afl/string/char.hpp"

namespace {
    /** Post-parse validation.
        \param c   [in] Original input string
        \param end [in] End pointer reported by strtoX
        \param pos [out] "first invalid character" pointer for user
        \param refuseNegative [in] If true, attempt to refuse negative numbers
        \retval true ok, return success to caller
        \retval false return error to caller */
    bool parsePostValidate(const char* c, const char* end, String_t::size_type& pos, bool refuseNegative)
    {
        // Check error
        if (errno == ERANGE) {
            pos = 0;
            return false;
        }

        // Refuse signs.
        // This is a hack: It is not an error for strtoull etc. to accept a negative number,
        // so if given one, they will happily accept it. We do not want negative numbers.
        // The only way to get a negative number is if the subject sequence [c,end)
        // contains a '-', so let's look for one and if we find one, refuse the element.
        if (refuseNegative) {
            if (const void* pc = std::memchr(c, '-', end-c)) {
                pos = static_cast<const char*>(pc) - c;
                return false;
            }
        }

        // Skip whitespace
        while (*end != '\0' && afl::string::charIsSpace(*end)) {
            ++end;
        }

        // Partially valid?
        if (*c == '\0' || *end != '\0') {
            pos = end - c;
            return false;
        } else {
            pos = 0;
            return true;
        }
    }

    /*
     *  The following functions implement strtoull/strtoll for compilers that don't have them.
     *  We implement only the nuances we need for our contract.
     *  The "..." argument makes sure the compiler prefers the library version - if any - over these.
     */

    bool readDigits(const char*& in, unsigned long long& result)
    {
        bool ok = false;
        unsigned long long value = 0;
        unsigned long long limit = -1ULL;
        while (*in >= '0' && *in <= '9') {
            unsigned int digit = *in - '0';
            if (value > limit/10 || digit > limit - 10*value) {
                // overflow!
                return false;
            }
            value = 10*value + digit;
            ++in;
            ok = true;
        }
        if (!ok) {
            // no digits!
            return false;
        }
        result = value;
        return true;
    }

    inline unsigned long long strtoull(const char* in, char** end, ...)
    {
        // Skip leading space
        while (afl::string::charIsSpace(*in)) {
            ++in;
        }

        // Skip '+', if any
        if (*in == '+') {
            ++in;
        }

        // Read digits
        unsigned long long result;
        if (!readDigits(in, result)) {
            errno = ERANGE;
            return 0;
        } else {
            *end = const_cast<char*>(in);
            return result;
        }
    }

    inline long long strtoll(const char* in, char** end, ...)
    {
        // Skip leading space
        while (afl::string::charIsSpace(*in)) {
            ++in;
        }

        // Skip sign, if any
        bool negative = false;
        if (*in == '+') {
            ++in;
        } else if (*in == '-') {
            ++in;
            negative = true;
        }

        // Read digits
        unsigned long long result;
        unsigned long long max = 1ULL << ((sizeof(unsigned long long) * CHAR_BIT) - 1);
        if (!readDigits(in, result)) {
            errno = ERANGE;
            return 0;
        } else if (negative) {
            if (result > max) {
                errno = ERANGE;
                return 0;
            } else {
                *end = const_cast<char*>(in);
                return static_cast<long long>(-result);
            }
        } else {
            if (result >= max) {
                errno = ERANGE;
                return 0;
            } else {
                *end = const_cast<char*>(in);
                return static_cast<long long>(result);
            }
        }
    }
}

// Parse using strtol
bool
afl::string::strToInteger(const String_t& str, long& value, String_t::size_type& pos)
{
    const char* c = str.c_str();
    char* end;

    // Parse using libc
    errno = 0;
    long v = std::strtol(c, &end, 10);
    if (parsePostValidate(c, end, pos, false)) {
        value = v;
        return true;
    } else {
        return false;
    }
}

// Parse using strtoul
bool
afl::string::strToInteger(const String_t& str, unsigned long& value, String_t::size_type& pos)
{
    const char* c = str.c_str();
    char* end;

    // Parse using libc
    errno = 0;
    unsigned long v = std::strtoul(c, &end, 10);
    if (parsePostValidate(c, end, pos, true)) {
        value = v;
        return true;
    } else {
        return false;
    }
}

bool
afl::string::strToInteger(const String_t& str, long long& value, String_t::size_type& pos)
{
    using namespace std;
    const char* c = str.c_str();
    char* end = 0;

    // Parse using libc or own
    errno = 0;
    long long v = strtoll(c, &end, 10);
    if (parsePostValidate(c, end, pos, false)) {
        value = v;
        return true;
    } else {
        return false;
    }
}

bool
afl::string::strToInteger(const String_t& str, unsigned long long& value, String_t::size_type& pos)
{
    using namespace std;
    const char* c = str.c_str();
    char* end = 0;

    // Parse using libc or own
    errno = 0;
    unsigned long long v = strtoull(c, &end, 10);
    if (parsePostValidate(c, end, pos, true)) {
        value = v;
        return true;
    } else {
        return false;
    }
}

bool
afl::string::strToFloat(const String_t& str, double& value, String_t::size_type& pos)
{
    const char* c = str.c_str();
    char* end;

    // Parse using libc
    errno = 0;
    double v = std::strtod(c, &end);
    if (parsePostValidate(c, end, pos, false)) {
        value = v;
        return true;
    } else {
        return false;
    }
}

bool
afl::string::strToFloat(const String_t& str, double& value)
{
    String_t::size_type pos;
    return strToFloat(str, value, pos);
}
