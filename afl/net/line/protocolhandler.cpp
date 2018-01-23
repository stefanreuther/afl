/**
  *  \file afl/net/line/protocolhandler.cpp
  *  \brief Class afl::net::line::ProtocolHandler
  */

#include "afl/net/line/protocolhandler.hpp"
#include "afl/net/line/linehandler.hpp"
#include "afl/net/line/linesink.hpp"
#include "afl/string/string.hpp"
#include "afl/sys/types.hpp"

afl::net::line::ProtocolHandler::ProtocolHandler(LineHandler& lineHandler)
    : afl::net::ProtocolHandler(),
      m_lineHandler(lineHandler),
      m_input(),
      m_opened(false),
      m_closeRequested(false),
      m_currentOutput(),
      m_futureOutput()
{ }

afl::net::line::ProtocolHandler::~ProtocolHandler()
{ }

void
afl::net::line::ProtocolHandler::getOperation(Operation& op)
{
    // At this time, m_currentOutput has been sent, so drop it
    m_currentOutput.reset();

    // Process opening, if any
    if (!m_opened) {
        m_opened = true;
        if (m_lineHandler.handleOpening(*this)) {
            m_closeRequested = true;
        }
    }

    // Now, if m_futureOutput is set, send that
    if (m_futureOutput.get() != 0) {
        // auto_ptr, so this is actually a move
        m_currentOutput = m_futureOutput;

        // prepare output
        op.m_dataToSend = *m_currentOutput;
        op.m_close = false;
        op.m_timeToWait = afl::sys::INFINITE_TIMEOUT;
    } else if (m_closeRequested) {
        // Close requested?
        op.m_dataToSend = afl::base::ConstBytes_t();
        op.m_close = true;
        op.m_timeToWait = 0;
    } else {
        // Nothing to do, receive.
        op.m_dataToSend = afl::base::ConstBytes_t();
        op.m_close = false;
        op.m_timeToWait = afl::sys::INFINITE_TIMEOUT;
    }
}

void
afl::net::line::ProtocolHandler::advanceTime(afl::sys::Timeout_t /*msecs*/)
{ }

void
afl::net::line::ProtocolHandler::handleData(afl::base::ConstBytes_t bytes)
{
    while (!bytes.empty() && !m_closeRequested) {
        // Convert to lines.
        // FIXME: make this a configurable character set conversion?
        m_input.append(afl::string::fromBytes(bytes.split(bytes.find('\n'))));

        // If bytes is not empty, we're looking at a '\n'.
        if (!bytes.empty()) {
            // Remove the '\n'
            bytes.split(1);

            // Process the string
            if (!m_input.empty() && m_input[m_input.size()-1] == '\r') {
                m_input.erase(m_input.size()-1);
            }
            if (m_lineHandler.handleLine(m_input, *this)) {
                m_closeRequested = true;
            }
            m_input.clear();
        }
    }
}

void
afl::net::line::ProtocolHandler::handleSendTimeout(afl::base::ConstBytes_t /*unsentBytes*/)
{
    // Should not happen, but indicates a connection problem. Just close it then.
    m_currentOutput.reset();
    m_futureOutput.reset();

    // For the LineHandler, this looks like an unvoluntary connection close.
    handleConnectionClose();
}

void
afl::net::line::ProtocolHandler::handleConnectionClose()
{
    if (!m_closeRequested) {
        m_closeRequested = true;
        m_lineHandler.handleConnectionClose();
    }
}

void
afl::net::line::ProtocolHandler::handleLine(const String_t& line)
{
    static const uint8_t CRLF[] = {'\r','\n'};
    if (m_futureOutput.get() == 0) {
        m_futureOutput.reset(new afl::base::GrowableBytes_t());
    }
    m_futureOutput->append(afl::string::toBytes(line));
    m_futureOutput->append(CRLF);
}
