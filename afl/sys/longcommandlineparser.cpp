/**
  *  \file afl/sys/longcommandlineparser.cpp
  */

#include "afl/sys/longcommandlineparser.hpp"


afl::sys::LongCommandLineParser::LongCommandLineParser(afl::base::Ptr<Environment::CommandLine_t> commandLine)
    : m_commandLine(commandLine),
      m_state(Idle),
      m_currentValue()
{ }

afl::sys::LongCommandLineParser::~LongCommandLineParser()
{ }

bool
afl::sys::LongCommandLineParser::getNext(bool& option, String_t& text)
{
    // End seen?
    if (m_state == End) {
        option = false;
        return m_commandLine->getNextElement(text);
    }

    // Fetch next parameter.
    if (!m_commandLine->getNextElement(m_currentValue)) {
        return false;
    }

    // '' or '-' always are options
    if (m_currentValue.empty() || m_currentValue == "-") {
        text = m_currentValue;
        option = false;
        m_state = Idle;
        return true;
    }

    // '--' produces nonoptions
    if (m_currentValue == "--") {
        option = false;
        m_state = End;
        return m_commandLine->getNextElement(text);
    }

    // Check for long options
    size_t kind = (m_currentValue[0] == '-'
                   ? (m_currentValue.size() > 2 && m_currentValue[1] == '-'
                      ? 2
                      : 1)
                   : 0);
    if (kind != 0) {
        // Option
        String_t::size_type eq = m_currentValue.find('=');
        if (eq != String_t::npos) {
            // '--foo=bar'
            text.assign(m_currentValue, kind, eq-kind);
            m_currentValue.erase(0, eq+1);
            m_state = WordParam;
        } else {
            // '--foo'
            text.assign(m_currentValue, kind, m_currentValue.size()-kind);
            m_state = Word;
        }
        option = true;
        return true;
    } else {
        // Nonoption
        m_state = Idle;
        text = m_currentValue;
        option = false;
        return true;
    }
}

bool
afl::sys::LongCommandLineParser::getParameter(String_t& value)
{
    State st = m_state;
    m_state = Idle;
    switch (st) {
     case Idle:
        return false;
     case Word:
        return m_commandLine->getNextElement(value);
     case WordParam:
        value = m_currentValue;
        return true;
     case End:
        return false;
    }
    return false;
}

afl::sys::CommandLineParser::Flags_t
afl::sys::LongCommandLineParser::getFlags()
{
    Flags_t result;
    switch (m_state) {
     case Idle:
        break;
     case Word:
        result += IsLongOption;
        break;
     case WordParam:
        result += IsLongOption;
        result += HasParameter;
        break;
     case End:
        break;
    }
    return result;
}
