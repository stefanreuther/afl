/**
  *  \file afl/sys/semaphore.cpp
  *  \brief Class afl::sys::Semaphore
  */

#include "afl/sys/semaphore.hpp"
#include "arch/semaphore.hpp"

afl::sys::Semaphore::Semaphore(int value)
    : m_pImpl(new Impl(value))
{ }

afl::sys::Semaphore::~Semaphore()
{
    delete m_pImpl;
}

void
afl::sys::Semaphore::post()
{
    m_pImpl->post();
}

void
afl::sys::Semaphore::wait()
{
    m_pImpl->wait();
}

bool
afl::sys::Semaphore::wait(Timeout_t timeout)
{
    return m_pImpl->wait(timeout);
}
