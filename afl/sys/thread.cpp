/**
  *  \file afl/sys/thread.cpp
  *  \brief class afl::sys::Thread
  */

#include <cassert>
#include "afl/sys/thread.hpp"
#include "afl/sys/mutexguard.hpp"
#include "arch/thread.hpp"
#include "afl/string/translator.hpp"

afl::sys::Thread::Thread(const char* name, afl::base::Runnable& r)
    : m_stateMutex(),
      m_state(NotStarted),
      m_runnable(r),
      m_name(name),
      m_pImpl(new Impl(this))
{ }

afl::sys::Thread::Thread(const String_t& name, afl::base::Runnable& r)
    : m_stateMutex(),
      m_state(NotStarted),
      m_runnable(r),
      m_name(name),
      m_pImpl(new Impl(this))
{ }

afl::sys::Thread::~Thread()
{
    join();
    delete m_pImpl;
}

void
afl::sys::Thread::start()
{
    // We are about to go multithreaded.
    // To avoid multiple threads attempting to create a translator in parallel,
    // poke it once to get it created if the user didn't already do it.
    afl::string::Translator::getSystemInstance();

    // Now do the actual work.
    {
        MutexGuard g(m_stateMutex);
        assert(m_state == NotStarted);
        m_state = Started;
    }
    m_pImpl->start();
}

void
afl::sys::Thread::join()
{
    MutexGuard g(m_stateMutex);
    if (m_state == Started) {
        m_pImpl->join();
    }
    m_state = Finished;
}

void
afl::sys::Thread::sleep(Timeout_t time)
{
    Impl::sleep(time);
}
