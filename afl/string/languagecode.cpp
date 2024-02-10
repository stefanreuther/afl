/**
  *  \file afl/string/languagecode.cpp
  */

#include <cstring>
#include "afl/string/languagecode.hpp"
#include "afl/string/char.hpp"

namespace {
    // Delimiters:
    //  en_US
    //  en-US
    //  en-US.UTF-8
    //  de_DE@euro
    const char*const DELIMITERS = "_:-/.@";

    bool isDelimiter(const char ch)
    {
        return std::strchr(DELIMITERS, ch) != 0;
    }
}


afl::string::LanguageCode::LanguageCode()
    : m_code()
{ }

afl::string::LanguageCode::LanguageCode(const String_t& code)
    : m_code(code)
{ }

const String_t&
afl::string::LanguageCode::get() const
{
    return m_code;
}

String_t
afl::string::LanguageCode::getWithDelimiter(char ch) const
{
    String_t result = m_code;
    for (size_t i = 0, n = result.size(); i < n; ++i) {
        if (isDelimiter(result[i])) {
            result[i] = ch;
        }
    }
    return result;
}

bool
afl::string::LanguageCode::isValid() const
{
    return !m_code.empty();
}

bool
afl::string::LanguageCode::operator==(const LanguageCode& other) const
{
    return m_code == other.m_code;
}

bool
afl::string::LanguageCode::operator!=(const LanguageCode& other) const
{
    return !operator==(other);
}

bool
afl::string::LanguageCode::operator<(const LanguageCode& other) const
{
    return m_code < other.m_code;
}

bool
afl::string::LanguageCode::operator<=(const LanguageCode& other) const
{
    return !other.operator<(*this);
}

bool
afl::string::LanguageCode::operator>(const LanguageCode& other) const
{
    return other.operator<(*this);
}

bool
afl::string::LanguageCode::operator>=(const LanguageCode& other) const
{
    return !operator<(other);
}

bool
afl::string::LanguageCode::generalize()
{
    size_t pos = m_code.size();

    // Special case for "UTF-8": skip single digit-only token
    // Multiple digits are still taken as a full token, as in "de-1996".
    if (pos >= 2 && charIsDigit(m_code[pos-1]) && isDelimiter(m_code[pos-2])) {
        pos -= 2;
    }

    // Find delimiter
    while (pos > 0) {
        --pos;
        if (isDelimiter(m_code[pos])) {
            break;
        }
    }

    // Check for "no single letter tokens" rule
    // RfC 4646 has this rule for 'x' and 'i' tokens; we accept all letters.
    // Do not accept digits to avoid chopping off the "-8" of an "UTF-8".
    while (pos >= 2 && (charIsUpper(m_code[pos-1]) || charIsLower(m_code[pos-1])) && isDelimiter(m_code[pos-2])) {
        pos -= 2;
    }

    // Found valid result?
    // Need to check '>1' because 1 would mean we leave a one-character token
    if (pos > 1) {
        m_code.erase(pos);
        return true;
    } else {
        return false;
    }
}

bool
afl::string::LanguageCode::isSameOrMoreGeneral(const LanguageCode& other) const
{
    return (m_code.size() == other.m_code.size()
            || (m_code.size() < other.m_code.size()
                && isDelimiter(other.m_code[m_code.size()])))
        && other.m_code.compare(0, m_code.size(), m_code) == 0;
}
