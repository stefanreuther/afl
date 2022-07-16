/**
  *  \file afl/test/loglistener.cpp
  *  \brief Class afl::test::LogListener
  */

#include "afl/test/loglistener.hpp"

afl::test::LogListener::LogListener()
    : m_numMessages(),
      m_numErrors(),
      m_numWarnings()
{ }

void
afl::test::LogListener::clear()
{
    m_numMessages = 0;
    m_numErrors = 0;
    m_numWarnings = 0;
}

size_t
afl::test::LogListener::getNumMessages() const
{
    return m_numMessages;
}

size_t
afl::test::LogListener::getNumErrors() const
{
    return m_numErrors;
}

size_t
afl::test::LogListener::getNumWarnings() const
{
    return m_numWarnings;
}

void
afl::test::LogListener::handleMessage(const Message& msg)
{
    ++m_numMessages;
    if (msg.m_level == Error) {
        ++m_numErrors;
    }
    if (msg.m_level == Warn) {
        ++m_numWarnings;
    }
}
