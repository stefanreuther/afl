/**
  *  \file afl/async/messageexchange.cpp
  *  \brief Class afl::async::MessageExchange
  */

#include "afl/async/messageexchange.hpp"
#include "afl/sys/mutexguard.hpp"
#include "afl/async/controller.hpp"
#include "afl/async/notifier.hpp"

afl::async::MessageExchange::MessageExchange()
    : m_mutex(),
      m_pendingSends(),
      m_pendingReceives()
{ }

afl::async::MessageExchange::~MessageExchange()
{ }

bool
afl::async::MessageExchange::send(Controller& ctl, SendOperation& op, afl::sys::Timeout_t timeout)
{
    sendAsync(ctl, op);
    if (!ctl.wait(op, timeout)) {
        cancel(ctl, op);
        return false;
    } else {
        return true;
    }
}

void
afl::async::MessageExchange::sendAsync(Controller& ctl, SendOperation& op)
{
    afl::sys::MutexGuard g(m_mutex);
    op.setController(&ctl);
    if (ReceiveOperation* rx = m_pendingReceives.extractFront()) {
        // There is a pending receive; pair it.
        rx->copyFrom(op);
        rx->getNotifier().notify(*rx);
        op.getNotifier().notifyDirect(op);
    } else {
        // No pending receive; wait.
        m_pendingSends.pushBack(&op);
    }
}

bool
afl::async::MessageExchange::receive(Controller& ctl, ReceiveOperation& op, afl::sys::Timeout_t timeout)
{
    receiveAsync(ctl, op);
    if (!ctl.wait(op, timeout)) {
        cancel(ctl, op);
        return false;
    } else {
        return true;
    }
}

void
afl::async::MessageExchange::receiveAsync(Controller& ctl, ReceiveOperation& op)
{
    afl::sys::MutexGuard g(m_mutex);
    op.setController(&ctl);
    if (SendOperation* tx = m_pendingSends.extractFront()) {
        // There is a pending send; pair it.
        op.copyFrom(*tx);
        tx->getNotifier().notify(*tx);
        op.getNotifier().notifyDirect(op);
    } else {
        // No pending send; wait.
        m_pendingReceives.pushBack(&op);
    }
}

void
afl::async::MessageExchange::cancel(Controller& ctl, Operation& op)
{
    afl::sys::MutexGuard g(m_mutex);
    m_pendingSends.remove(&op);
    m_pendingReceives.remove(&op);
    ctl.revertPost(op);
}

String_t
afl::async::MessageExchange::getName()
{
    return "<MessageExchange>";
}
