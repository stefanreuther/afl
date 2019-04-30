/**
  *  \file afl/sys/standardcommandlineparser.cpp
  *  \brief Class afl::sys::StandardCommandLineParser
  */

#include "afl/sys/standardcommandlineparser.hpp"

afl::sys::StandardCommandLineParser::StandardCommandLineParser(afl::base::Ref<Environment::CommandLine_t> commandLine)
    : m_commandLine(commandLine),
      m_state(Idle),
      m_currentValue(),
      m_currentIndex(0)
{ }

afl::sys::StandardCommandLineParser::~StandardCommandLineParser()
{ }

bool
afl::sys::StandardCommandLineParser::getNext(bool& option, String_t& text)
{
    // Early-out cases
    switch (m_state) {
     case Characters:
        // If we are in character mode, we may be able to produce an option right now.
        if (m_currentIndex < m_currentValue.size()) {
            option = true;
            text.assign(1, m_currentValue[m_currentIndex++]);
            return true;
        }
        break;

     case End:
        // We only produce nonoptions
        option = false;
        return m_commandLine->getNextElement(text);
        break;

     case Idle:
     case Word:
     case WordParam:
        break;
    }

    // Cannot produce an option; fetch next parameter.
    if (!m_commandLine->getNextElement(m_currentValue)) {
        return false;
    }

    // '' or '-' always are nonoptions
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
    if (m_currentValue.size() > 2 && m_currentValue.compare(0, 2, "--", 2) == 0) {
        String_t::size_type eq = m_currentValue.find('=');
        if (eq != String_t::npos) {
            // '--foo=bar'
            text.assign(m_currentValue, 2, eq-2);
            m_currentValue.erase(0, eq+1);
            m_state = WordParam;
        } else {
            // '--foo'
            text.assign(m_currentValue, 2, m_currentValue.size()-2);
            m_state = Word;
        }
        option = true;
        return true;
    }

    // Check for short options
    if (m_currentValue[0] == '-') {
        text.assign(m_currentValue, 1, 1);
        m_currentIndex = 2;
        m_state = Characters;
        option = true;
        return true;
    }

    // OK, must be nonoption
    m_state = Idle;
    text = m_currentValue;
    option = false;
    return true;
}

bool
afl::sys::StandardCommandLineParser::getParameter(String_t& value)
{
    State st = m_state;
    m_state = Idle;
    switch (st) {
     case Idle:
        return false;
     case Characters:
        if (m_currentIndex >= m_currentValue.size()) {
            return m_commandLine->getNextElement(value);
        } else {
            value.assign(m_currentValue, m_currentIndex, m_currentValue.size() - m_currentIndex);
            return true;
        }
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
afl::sys::StandardCommandLineParser::getFlags()
{
    Flags_t result;
    switch (m_state) {
     case Idle:
        break;
     case Characters:
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
