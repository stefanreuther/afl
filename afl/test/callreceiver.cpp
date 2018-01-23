/**
  *  \file afl/test/callreceiver.cpp
  *  \brief Class afl::test::CallReceiver
  */

#include "afl/test/callreceiver.hpp"
#include "afl/string/format.hpp"

using afl::string::Format;

afl::test::CallReceiver::CallReceiver(const Assert& a)
    : m_assert(a),
      m_queue(),
      m_returnValues(),
      m_nextReturnValue(0)
{ }

afl::test::CallReceiver::~CallReceiver()
{ }

void
afl::test::CallReceiver::expectCall(String_t call)
{
    m_queue.push_back(call);
}

void
afl::test::CallReceiver::checkCall(String_t call)
{
    m_assert.check(Format("checkCall(%s): need pending call", call), !m_queue.empty());
    m_assert.checkEqual("checkCall", m_queue.front(), call);
    m_queue.pop_front();
}

void
afl::test::CallReceiver::checkFinish()
{
    m_assert.check("checkFinish: must have no more pending calls", m_queue.empty());
    m_assert.checkEqual("checkFinish: must have no more pending returns", m_nextReturnValue, m_returnValues.size());
}
