/**
  *  \file afl/net/http/formparser.cpp
  */

#include "afl/net/http/formparser.hpp"
#include "afl/charset/urlencoding.hpp"

afl::net::http::FormParser::FormParser(HeaderConsumer& consumer)
    : m_consumer(consumer),
      m_state(Key),
      m_key(),
      m_value()
{ }

afl::net::http::FormParser::~FormParser()
{ }

void
afl::net::http::FormParser::handleDataComplete()
{
    if (!m_key.empty() || !m_value.empty()) {
        afl::charset::UrlEncoding ue;
        m_consumer.handleHeader(ue.decode(afl::string::toBytes(m_key)), ue.decode(afl::string::toBytes(m_value)));
        m_key.clear();
        m_value.clear();
    }
    m_state = Key;
}

// DataSink:
bool
afl::net::http::FormParser::handleData(afl::base::ConstBytes_t& data)
{
    while (const uint8_t* p = data.eat()) {
        if (*p == '&') {
            // '&' appearing without a preceding '=' is not valid.
            // We still process it here in either state to match the behaviour of Url::matchArguments().
            handleDataComplete();
        } else if (m_state == Key) {
            if (*p == '=') {
                m_state = Value;
            } else {
                m_key.append(1, char(*p));
            }
        } else {
            m_value.append(1, char(*p));
        }
    }
    return false;
}
