/**
  *  \file afl/net/parameterencoder.cpp
  *  \brief Class afl::net::ParameterEncoder
  */

#include "afl/net/parameterencoder.hpp"
#include "afl/charset/urlencoding.hpp"

afl::net::ParameterEncoder::ParameterEncoder(String_t& result, char firstDelimiter)
    : m_result(result),
      m_first(true),
      m_firstDelimiter(firstDelimiter)
{ }

void
afl::net::ParameterEncoder::handleHeader(String_t key, String_t value)
{
    if (m_first) {
        if (m_firstDelimiter != '\0') {
            m_result += m_firstDelimiter;
        }
        m_first = false;
    } else {
        m_result += "&";
    }
    m_result += afl::string::fromBytes(afl::charset::UrlEncoding().encode(afl::string::toMemory(key)));
    m_result += "=";
    m_result += afl::string::fromBytes(afl::charset::UrlEncoding().encode(afl::string::toMemory(value)));
}
