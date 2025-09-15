/**
  *  \file afl/async/internalinterrupt.cpp
  *  \brief Class afl::async::InternalInterrupt
  */

#include "afl/async/internalinterrupt.hpp"
#include "afl/async/controller.hpp"
#include "afl/async/notifier.hpp"
#include "afl/sys/mutexguard.hpp"

afl::async::InternalInterrupt::InternalInterrupt()
    : m_mutex(),
      m_waiters()
{ }

afl::async::InternalInterrupt::~InternalInterrupt()
{ }

afl::async::InterruptOperation::Kinds_t
afl::async::InternalInterrupt::wait(Controller& ctl, InterruptOperation::Kinds_t kinds, afl::sys::Timeout_t timeout)
{
    InterruptOperation op(kinds);
    waitAsync(ctl, op);
    if (!ctl.wait(op, timeout)) {
        cancel(ctl, op);
        return InterruptOperation::Kinds_t();
    } else {
        return op.getReceived();
    }
}

void
afl::async::InternalInterrupt::waitAsync(Controller& ctl, InterruptOperation& op)
{
    afl::sys::MutexGuard g(m_mutex);
    op.setController(&ctl);
    m_waiters.push_back(&op);
}

void
afl::async::InternalInterrupt::cancel(Controller& ctl, Operation& op)
{
    afl::sys::MutexGuard g(m_mutex);
    std::list<InterruptOperation*>::iterator it = m_waiters.begin();
    while (it != m_waiters.end()) {
        if ((*it)->getController() == &ctl) {
            it = m_waiters.erase(it);
        } else {
            ++it;
        }
    }
    ctl.revertPost(op);
}

void
afl::async::InternalInterrupt::post(InterruptOperation::Kinds_t kinds)
{
    afl::sys::MutexGuard g(m_mutex);
    std::list<InterruptOperation*>::iterator it = m_waiters.begin();
    while (it != m_waiters.end()) {
        InterruptOperation* op = *it;
        if (op->getRequested().containsAnyOf(kinds)) {
            op->setReceived(op->getRequested() & kinds);
            it = m_waiters.erase(it);
            op->getNotifier().notify(*op);
        } else {
            ++it;
        }
    }
}
