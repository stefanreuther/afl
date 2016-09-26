/**
  *  \file afl/net/headerparser.cpp
  *  \brief Class afl::net::HeaderParser
  */

#include "afl/net/headerparser.hpp"
#include "afl/net/headerconsumer.hpp"

afl::net::HeaderParser::HeaderParser(HeaderConsumer& consumer)
    : m_consumer(consumer),
      m_hasErrors(false),
      m_hasFolding(false),
      m_key(),
      m_value(),
      m_state(InitialState)
{ }

afl::net::HeaderParser::~HeaderParser()
{ }

bool
afl::net::HeaderParser::handleData(const String_t& /*name*/, afl::base::ConstBytes_t& bytes)
{
    const uint8_t* p;
    while (m_state != FinalState && (p = bytes.eat()) != 0) {
        switch (*p) {
         case '\0':
            // Invalid everywhere
            m_hasErrors = true;
            break;

         case '\r':
            // Ignored everywhere
            break;

         case '\n':
            switch (m_state) {
             case InitialState:
                m_state = FinalState;
                break;
             case KeyState:
                m_hasErrors = true;
                m_key.clear();
                m_state = InitialState;
                break;
             case ValueState:
                m_state = NewlineState;
                break;
             case NewlineState:
                flush();
                m_state = FinalState;
                break;
             case ErrorState:
                m_key.clear();
                m_state = InitialState;
                break;
             case FinalState:
                break;
            }
            break;

         case ' ':
         case '\t':
            switch (m_state) {
             case InitialState:
             case KeyState:
                m_key.clear();
                m_hasErrors = true;
                m_state = ErrorState;
                break;
                
             case ValueState:
                if (!m_value.empty()) {
                    m_value.append(1, char(*p));
                }
                break;

             case NewlineState:
                m_hasFolding = true;
                m_state = ValueState;
                if (!m_value.empty()) {
                    m_value.append(1, char(*p));
                }
                break;

             case ErrorState:
             case FinalState:
                break;
            }
            break;

         case ':':
            switch (m_state) {
             case InitialState:
                m_hasErrors = true;
                m_state = ErrorState;
                break;
             case KeyState:
                m_state = ValueState;
                break;

             case ValueState:
                m_value.append(1, char(*p));
                break;

             case NewlineState:
                flush();
                m_hasErrors = true;
                m_state = ErrorState;
                break;

             case ErrorState:
             case FinalState:
                break;
            }
            break;

         default:
            switch (m_state) {
             case InitialState:
                m_key.append(1, char(*p));
                m_state = KeyState;
                break;

             case KeyState:
                m_key.append(1, char(*p));
                break;

             case ValueState:
                m_value.append(1, char(*p));
                break;

             case NewlineState:
                flush();
                m_key.append(1, char(*p));
                m_state = KeyState;
                break;

             case ErrorState:
             case FinalState:
                break;
            }
            break;
        }
    }
    return m_state == FinalState;
}

void
afl::net::HeaderParser::flush()
{
    m_consumer.handleHeader(m_key, m_value);
    m_key.clear();
    m_value.clear();
}
