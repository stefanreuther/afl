/**
  *  \file afl/base/signalhandler.cpp
  *  \brief Base Class afl::base::SignalHandler
  */

#include <cassert>
#include "afl/base/signalhandler.hpp"

afl::base::SignalHandler::~SignalHandler()
{
    assert(m_pNext);
    assert(*m_pNext == this);
    if (m_next) {
        assert(m_next->m_pNext == &m_next);
    }

    if (m_next) {
        m_next->m_pNext = m_pNext;
    }
    *m_pNext = m_next;

    if (m_pBacklink) {
        assert(*m_pBacklink == this);
        *m_pBacklink = 0;
    }
}

void
afl::base::SignalHandler::backlink(SignalHandler** pBacklink)
{
    assert(m_pBacklink == 0);
    assert(*pBacklink == this);
    m_pBacklink = pBacklink;
}
