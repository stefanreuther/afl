/**
  *  \file afl/net/headerfield.cpp
  *  \brief Class afl::net::HeaderField
  */

#include "afl/net/headerfield.hpp"
#include "afl/string/string.hpp"
#include "afl/net/headerconsumer.hpp"

namespace {
    /** Parse part of a header, up to a semicolon.
        This interprets double-quoting, comments (parentheses) and backslash escaping.
        It stops when encountering an unquoted semicolon.
        \param value [in] String to parse
        \param pos   [in/out] Read pointer
        \param mode  [in] Parse mode (NoComments, NoQuotes)
        \return header part */
    String_t
    extractPart(const String_t& value, String_t::size_type& pos, const int mode)
    {
        String_t result;
        enum { Raw, DQ, DQQ, Paren, ParenQ } state = Raw;
        while (pos < value.size()) {
            const char c = value[pos++];
            switch (state) {
             case Raw:
                if (c == '"' && (mode & afl::net::HeaderField::NoQuotes) == 0) {
                    state = DQ;
                } else if (c == '(' && (mode & afl::net::HeaderField::NoComments) == 0) {
                    state = Paren;
                } else if (c == ';') {
                    goto out;
                } else {
                    result += c;
                }
                break;
             case DQ:
                if (c == '\\') {
                    state = DQQ;
                } else if (c == '"') {
                    state = Raw;
                } else {
                    result += c;
                }
                break;
             case DQQ:
                result += c;
                state = DQ;
                break;
             case Paren:
                if (c == ')') {
                    state = Raw;
                } else if (c == '\\') {
                    state = ParenQ;
                } else {
                    // ignore
                }
                break;
             case ParenQ:
                state = Paren;
                break;
            }
        }
     out:
        return result;
    }
}

afl::net::HeaderField::HeaderField()
    : m_name(),
      m_value()
{ }

afl::net::HeaderField::HeaderField(const String_t& name, const String_t& value)
    : m_name(name),
      m_value(value)
{ }

afl::net::HeaderField::~HeaderField()
{ }

const String_t&
afl::net::HeaderField::getName() const
{
    return m_name;
}

const String_t&
afl::net::HeaderField::getValue() const
{
    return m_value;
}

void
afl::net::HeaderField::setValue(const String_t& value)
{
    m_value = value;
}

String_t
afl::net::HeaderField::getPrimaryValue(int mode) const
{
    String_t::size_type pos = 0;
    return afl::string::strTrim(extractPart(m_value, pos, mode));
}

bool
afl::net::HeaderField::getSecondaryValue(const String_t& name, String_t& value, int mode) const
{
    struct Helper : public HeaderConsumer {
        Helper(const String_t& name, String_t& value)
            : m_name(name),
              m_value(value),
              m_result(false)
            { }

        void handleHeader(String_t key, String_t value)
            {
                if (!m_result && afl::string::strCaseCompare(key, m_name) == 0) {
                    m_value = value;
                    m_result = true;
                }
            }

        const String_t& m_name;
        String_t& m_value;
        bool m_result;
    };
    Helper h(name, value);
    enumerateSecondaryValues(h, mode);
    return h.m_result;
}

void
afl::net::HeaderField::enumerateSecondaryValues(HeaderConsumer& consumer, int mode) const
{
    String_t::size_type pos = 0;
    if ((mode & NoPrimary) == 0) {
        extractPart(m_value, pos, mode);
    }
    while (pos < m_value.size()) {
        // Get the part. This can be "a=b" or just "a".
        const String_t part(extractPart(m_value, pos, mode));

        // Check for '=' delimiter
        String_t::size_type pos = part.find('=');
        if (pos != String_t::npos) {
            consumer.handleHeader(afl::string::strTrim(part.substr(0, pos)), afl::string::strTrim(part.substr(pos+1)));
        } else {
            consumer.handleHeader(afl::string::strTrim(part), String_t());
        }
    }
}
