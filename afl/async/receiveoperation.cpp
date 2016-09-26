/**
  *  \file afl/async/receiveoperation.cpp
  *  \brief Class afl::async::ReceiveOperation
  */

#include "afl/async/receiveoperation.hpp"
#include "afl/async/sendoperation.hpp"

void
afl::async::ReceiveOperation::copyFrom(SendOperation& other)
{
    size_t bytesCopied = m_data.subrange(m_numReceivedBytes).copyFrom(other.getUnsentBytes()).size();
    m_numReceivedBytes += bytesCopied;
    other.addSentBytes(bytesCopied);
}
