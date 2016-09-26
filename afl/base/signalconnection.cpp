/**
  *  \file afl/base/signalconnection.cpp
  *  \brief Class afl::base::SignalConnection
  */

#include <cassert>
#include "afl/base/signalconnection.hpp"

afl::base::SignalConnection::SignalConnection(SignalHandler* handler)
    : m_handler(handler)
{
    if (m_handler) {
        m_handler->backlink(&m_handler);
    }
}

void
afl::base::SignalConnection::operator=(SignalHandler* newHandler)
{
    if (m_handler != newHandler) {
        delete m_handler;
        assert(m_handler == 0);
        m_handler = newHandler;
        if (m_handler) {
            m_handler->backlink(&m_handler);
        }
    }
}
