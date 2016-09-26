/**
  *  \file afl/async/controller.cpp
  *  \brief Class afl::async::Controller
  */

#include "afl/async/controller.hpp"
#include "afl/sys/mutexguard.hpp"
#include "afl/sys/time.hpp"
#include "afl/sys/types.hpp"
#include "arch/controller.hpp"


afl::async::Controller::Controller()
    : m_completedMutex(),
      m_completedOperations(),
      m_pImpl(new Impl())
{ }

afl::async::Controller::~Controller()
{
    // FIXME: clean up completed ops
    delete m_pImpl;
}

afl::async::Operation*
afl::async::Controller::wait(afl::sys::Timeout_t timeout)
{
    uint32_t ticks = afl::sys::Time::getTickCounter();
    while (1) {
        m_pImpl->prepare();

        // Check for completed operation
        {
            afl::sys::MutexGuard g(m_completedMutex);
            Operation* op = m_completedOperations.extractFront();
            if (op != 0) {
                m_pImpl->finish();
                return op;
            }
        }

        // Wait
        m_pImpl->wait(timeout);

        // Update timeout
        if (timeout != afl::sys::INFINITE_TIMEOUT) {
            uint32_t now = afl::sys::Time::getTickCounter();
            uint32_t elapsed = now - ticks;
            if (elapsed < timeout) {
                ticks = now;
                timeout -= elapsed;
            } else {
                m_pImpl->finish();
                return 0;
            }
        }
    }
}

bool
afl::async::Controller::wait(Operation& op, afl::sys::Timeout_t timeout)
{
    uint32_t ticks = afl::sys::Time::getTickCounter();
    while (1) {
        m_pImpl->prepare();

        // Check for completed operation
        {
            afl::sys::MutexGuard g(m_completedMutex);
            if (m_completedOperations.remove(&op)) {
                m_pImpl->finish();
                return true;
            }
        }

        // Wait
        m_pImpl->wait(timeout);

        // Update timeout
        if (timeout != afl::sys::INFINITE_TIMEOUT) {
            uint32_t now = afl::sys::Time::getTickCounter();
            uint32_t elapsed = now - ticks;
            if (elapsed < timeout) {
                ticks = now;
                timeout -= elapsed;
            } else {
                m_pImpl->finish();
                return false;
            }
        }
    }
}

void
afl::async::Controller::post(Operation& op)
{
    {
        afl::sys::MutexGuard g(m_completedMutex);
        m_completedOperations.pushBack(&op);
    }
    m_pImpl->wake();
}

void
afl::async::Controller::postDirect(Operation& op)
{
    post(op);
}

void
afl::async::Controller::revertPost(Operation& op)
{
    afl::sys::MutexGuard g(m_completedMutex);
    m_completedOperations.remove(&op);
}
