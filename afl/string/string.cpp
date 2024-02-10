/**
  *  \file afl/string/string.cpp
  *  \brief String functions
  */

#include <cstring>
#include "afl/string/string.hpp"
#include "afl/string/char.hpp"

/*
 *  Conversion to/from memory
 */

// Convert string to Memory
afl::string::ConstStringMemory_t
afl::string::toMemory(const String_t& str)
{
    String_t::size_type size = str.size();
    return size != 0
        ? ConstStringMemory_t::unsafeCreate(str.data(), size)
        : ConstStringMemory_t();
}

// Convert C string to Memory
afl::string::ConstStringMemory_t
afl::string::toMemory(const char* ntbs)
{
    return ntbs != 0
        ? ConstStringMemory_t::unsafeCreate(ntbs, std::strlen(ntbs))
        : ConstStringMemory_t();
}

// Convert C string to Memory, limited
afl::string::ConstStringMemory_t
afl::string::toMemoryLimited(const char* ntbs, size_t maxLen)
{
    if (ntbs == 0) {
        return ConstStringMemory_t();
    } else {
        const char* end = static_cast<const char*>(std::memchr(ntbs, 0, maxLen));
        if (end != 0) {
            maxLen = end - ntbs;
        }
        return ConstStringMemory_t::unsafeCreate(ntbs, maxLen);
    }
}

// Convert Memory to string (by copying)
String_t
afl::string::fromMemory(ConstStringMemory_t mem)
{
    String_t::size_type size = mem.size();
    return size != 0
        ? std::string(mem.unsafeData(), mem.size())
        : std::string();
}


afl::base::ConstBytes_t
afl::string::toBytes(const String_t& str)
{
    String_t::size_type size = str.size();
    return size != 0
        ? afl::base::ConstBytes_t::unsafeCreate(reinterpret_cast<const uint8_t*>(str.data()), size)
        : afl::base::ConstBytes_t();
}

afl::base::ConstBytes_t
afl::string::toBytes(const char* ntbs)
{
    return ntbs != 0
        ? afl::base::ConstBytes_t::unsafeCreate(reinterpret_cast<const uint8_t*>(ntbs), std::strlen(ntbs))
        : afl::base::ConstBytes_t();

}

String_t
afl::string::fromBytes(afl::base::ConstBytes_t mem)
{
    String_t::size_type size = mem.size();
    return size != 0
        ? std::string(reinterpret_cast<const char*>(mem.unsafeData()), mem.size())
        : std::string();
}


/*
 *  Case-insensitive comparison (ASCII only)
 */

// Compare (base version)
int
afl::string::strCaseCompare(ConstStringMemory_t a, ConstStringMemory_t b)
{
#if 0
    while (1) {
        const char* pa = a.eat();
        const char* pb = b.eat();

        if (pa == 0) {
            if (pb == 0) {
                // Equal (both ended)
                return 0;
            } else {
                // a ended, but b didn't: a comes first
                return -1;
            }
        } else {
            if (pb == 0) {
                // b ended, but a didn't: b comes first
                return 1;
            } else {
                // both have a character; compare that
                int diff = charToUpper(*pa) - charToUpper(*pb);
                if (diff != 0) {
                    return diff;
                }
            }
        }
    }
#else
    /* Slightly less idiomatic version.
       In c2ng, time for MovementPredictor test is dominated by strCaseCompare() (used for configuration access).
       This version reduces overall time for that test by 20% (19->15s). */
    size_t commonLength = std::min(a.size(), b.size());
    for (size_t i = 0; i < commonLength; ++i) {
        char ca = a.unsafeData()[i];
        char cb = b.unsafeData()[i];
        if (ca != cb) {
            int diff = charToUpper(ca) - charToUpper(cb);
            if (diff != 0) {
                return diff;
            }
        }
    }
    return (a.size() < b.size()
            ? -1
            : a.size() == b.size()
            ? 0
            : +1);
#endif
}

// Compare (C version)
int
afl::string::strCaseCompare(const char* a, const char* b)
{
    return strCaseCompare(toMemory(a), toMemory(b));
}

// Compare (C++ version)
int
afl::string::strCaseCompare(const String_t& a, const String_t& b)
{
    return strCaseCompare(toMemory(a), toMemory(b));
}

// Compare (mixed)
int
afl::string::strCaseCompare(const char* a, const String_t& b)
{
    return strCaseCompare(toMemory(a), toMemory(b));
}

// Compare (mixed)
int
afl::string::strCaseCompare(const String_t& a, const char* b)
{
    return strCaseCompare(toMemory(a), toMemory(b));
}


/*
 *  Trimming / Parsing
 */

// Trim both
String_t
afl::string::strTrim(String_t s)
{
    return strLTrim(strRTrim(s));
}

// Trim left
String_t
afl::string::strLTrim(String_t s)
{
    String_t::size_type n = 0;
    while (n < s.length() && charIsSpace(s[n])) {
        ++n;
    }
    return s.substr(n);
}

// Trim right
String_t
afl::string::strRTrim(String_t s)
{
    String_t::size_type n = s.length();
    while (n > 0 && charIsSpace(s[n-1])) {
        --n;
    }
    return s.substr(0, n);
}

// Get before separator
String_t
afl::string::strFirst(const String_t str, const String_t sep)
{
    String_t::size_type n = str.find(sep);
    if (n == String_t::npos) {
        return str;
    } else {
        return str.substr(0, n);
    }
}

// Remove up to separator
bool
afl::string::strRemove(String_t& str, const String_t sep)
{
    String_t::size_type n = str.find(sep);
    if (n == String_t::npos) {
        return false;
    } else {
        str.erase(0, n + sep.length());
        return true;
    }
}

// Split at separator
bool
afl::string::strSplit(const String_t str, String_t& lhs, String_t& rhs, const String_t sep)
{
    String_t::size_type n = str.find(sep);
    if (n == String_t::npos) {
        return false;
    } else {
        lhs = str.substr(0, n);
        rhs = str.substr(n+sep.length());
        return true;
    }
}

// Convert to lower case.
String_t
afl::string::strLCase(String_t s)
{
    for (String_t::size_type n = 0; n < s.length(); ++n) {
        s[n] = charToLower(s[n]);
    }
    return s;
}

// Convert to upper case
String_t
afl::string::strUCase(String_t s)
{
    for (String_t::size_type n = 0; n < s.length(); ++n) {
        s[n] = charToUpper(s[n]);
    }
    return s;
}

// Capitalize string
String_t
afl::string::strUCFirst(String_t s)
{
    bool uc = true;
    for (String_t::size_type n = 0; n < s.length(); ++n) {
        if (charIsSpace(s[n]) || s[n] == '-') {
            uc = true;
        } else {
            if (uc) {
                s[n] = charToUpper(s[n]);
            } else {
                s[n] = charToLower(s[n]);
            }
            uc = false;
        }
    }
    return s;
}

// Un-capitalize string
String_t
afl::string::strLCWords(String_t s)
{
    bool uc = true;
    for (String_t::size_type n = 0; n < s.length(); ++n) {
        if (charIsSpace(s[n]) || s[n] == '-') {
            uc = true;
        } else {
            if (!uc) {
                s[n] = charToLower(s[n]);
            }
            uc = false;
        }
    }
    return s;
}


/*
 *  Word extraction
 */

static String_t::size_type
strNthWordStart(const String_t& s, size_t i)
{
    String_t::size_type k = 0;

    // Skip leading space
    while (k < s.length() && afl::string::charIsSpace(s[k])) {
        ++k;
    }

    while (i > 0) {
        // Skip word and following space
        while (k < s.length() && !afl::string::charIsSpace(s[k])) {
            ++k;
        }
        while (k < s.length() && afl::string::charIsSpace(s[k])) {
            ++k;
        }
        --i;
    }
    return k;
}


/*
 *  Word extraction
 */

// Extract word
String_t
afl::string::strNthWord(const String_t& s, size_t i)
{
    String_t::size_type k = strNthWordStart(s, i);
    String_t::size_type j = k;
    while (j < s.length() && !charIsSpace(s[j])) {
        ++j;
    }
    return String_t(s, k, j-k);
}

// Extract word including all that follow
String_t
afl::string::strNthWordRest(const String_t& s, size_t i)
{
    return String_t(s, strNthWordStart(s, i));
}
