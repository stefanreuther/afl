/**
  *  \file afl/async/semaphore.cpp
  *  \brief Class afl::async::Semaphore
  */

#include <cassert>
#include "afl/async/semaphore.hpp"
#include "afl/async/controller.hpp"
#include "afl/async/operation.hpp"
#include "afl/sys/mutexguard.hpp"
#include "afl/async/notifier.hpp"

afl::async::Semaphore::Semaphore(uint32_t initialValue)
    : m_mutex(),
      m_waiters(),
      m_value(initialValue)
{ }

afl::async::Semaphore::~Semaphore()
{ }

void
afl::async::Semaphore::post()
{
    afl::sys::MutexGuard g(m_mutex);
    if (Operation* op = m_waiters.extractFront()) {
        // Got a waiter: signal it
        op->getNotifier().notify(*op);
    } else {
        // No waiter, just bump count
        ++m_value;
    }
}

bool
afl::async::Semaphore::wait(Controller& ctl, afl::sys::Timeout_t timeout)
{
    Operation op;
    waitAsync(ctl, op);
    if (!ctl.wait(op, timeout)) {
        // FIXME: for extra safety, deal with another thread releasing the semaphore between
        // the wait() and the cancel().
        cancel(ctl, op);
        return false;
    } else {
        return true;
    }
}

void
afl::async::Semaphore::waitAsync(Controller& ctl, Operation& op)
{
    afl::sys::MutexGuard g(m_mutex);
    op.setController(&ctl);
    if (m_value == 0) {
        // Must wait
        m_waiters.pushBack(&op);
    } else {
        // We have pending signal
        --m_value;
        op.getNotifier().notifyDirect(op);
    }
}

void
afl::async::Semaphore::cancel(Controller& ctl, Operation& op)
{
    afl::sys::MutexGuard g(m_mutex);
    m_waiters.remove(&op);
    ctl.revertPost(op);
}
