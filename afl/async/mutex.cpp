/**
  *  \file afl/async/mutex.cpp
  *  \brief Class afl::async::Mutex
  */

#include "afl/async/mutex.hpp"
#include "afl/sys/mutexguard.hpp"
#include "afl/async/operation.hpp"
#include "afl/async/controller.hpp"
#include "afl/async/notifier.hpp"


afl::async::Mutex::Mutex()
    : m_mutex(),
      m_waiters(),
      m_nestingLevel(0),
      m_pOwner(0)
{ }

afl::async::Mutex::~Mutex()
{ }

bool
afl::async::Mutex::wait(Controller& ctl, afl::sys::Timeout_t timeout)
{
    Operation op;
    waitAsync(ctl, op);
    if (!ctl.wait(op, timeout)) {
        cancel(ctl, op);
        return false;
    } else {
        return true;
    }
}

void
afl::async::Mutex::waitAsync(Controller& ctl, Operation& op)
{
    afl::sys::MutexGuard g(m_mutex);
    op.setController(&ctl);
    if (m_pOwner == &ctl) {
        // Nested acquisition
        ++m_nestingLevel;
        op.getNotifier().notifyDirect(op);
    } else if (m_pOwner == 0) {
        // Initial acquisition
        m_pOwner = &ctl;
        m_nestingLevel = 1;
        op.getNotifier().notifyDirect(op);
    } else {
        // Blocked
        m_waiters.pushBack(&op);
    }
}

void
afl::async::Mutex::post(Controller& ctl)
{
    afl::sys::MutexGuard g(m_mutex);
    if (m_pOwner == &ctl) {
        --m_nestingLevel;
        if (m_nestingLevel == 0) {
            if (Operation* op = m_waiters.extractFront()) {
                // We are transferring ownership to somebody else
                Controller* newOwner = op->getController();
                m_pOwner = newOwner;
                m_nestingLevel = 1;
                op->getNotifier().notify(*op);

                // They may be waiting multiple times; process nested acquisitions.
                while (Operation* op2 = m_waiters.extractByController(newOwner)) {
                    ++m_nestingLevel;
                    op2->getNotifier().notify(*op2);
                }
            } else {
                // We are releasing the mutex
                m_pOwner = 0;
            }
        }
    } else {
        // Error. We're releasing a mutex we don't own. Ignore this.
    }
}

void
afl::async::Mutex::cancel(Controller& ctl, Operation& op)
{
    afl::sys::MutexGuard g(m_mutex);
    m_waiters.remove(&op);
    ctl.revertPost(op);
}
