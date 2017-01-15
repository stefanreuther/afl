/**
  *  \file afl/sys/loglistener.cpp
  *  \brief Class afl::sys::LogListener
  */

#include "afl/sys/loglistener.hpp"
#include "afl/sys/log.hpp"
#include "afl/string/format.hpp"
#include "afl/except/fileproblemexception.hpp"

afl::sys::LogListener::LogListener()
    : m_parents()
{ }

afl::sys::LogListener::~LogListener()
{
    Log::killListener(*this);
}

void
afl::sys::LogListener::write(Level level, const String_t& channel, const String_t& message)
{
    Message msg;
    msg.m_level   = level;
    msg.m_time    = Time::getCurrentTime();
    msg.m_channel = channel;
    msg.m_message = message;
    handleMessage(msg);
}

void
afl::sys::LogListener::write(Level level, const String_t& channel, const String_t& fileName, int lineNr, const String_t& message)
{
    // The idea of this method is that we ultimately can pass on the information that the message starts with a file name,
    // to make it clickable in GUI listings.
    if (lineNr > 0) {
        write(level, channel, afl::string::Format("%s:%d: %s", fileName, lineNr, message));
    } else {
        write(level, channel, afl::string::Format("%s: %s", fileName, message));
    }
}

void
afl::sys::LogListener::write(Level level, const String_t& channel, const String_t& message, const std::exception& e)
{
    write(level, channel, formatException(message, e));
}

String_t
afl::sys::LogListener::formatException(const String_t& message, const std::exception& e)
{
    if (const afl::except::FileProblemException* fpe = dynamic_cast<const afl::except::FileProblemException*>(&e)) {
        if (!message.empty()) {
            return afl::string::Format("%s: %s: %s", fpe->getFileName(), message, e.what());
        } else {
            return afl::string::Format("%s: %s", fpe->getFileName(), e.what());
        }
    } else {
        if (!message.empty()) {
            return afl::string::Format("%s: %s", message, e.what());
        } else {
            return e.what();
        }
    }
}
