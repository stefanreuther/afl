/**
  *  \file arch/mutex.hpp
  *  \brief System-dependant Part of afl/sys/mutex.cpp
  */
#ifndef AFL_ARCH_MUTEX_HPP
#define AFL_ARCH_MUTEX_HPP

#include "afl/sys/mutex.hpp"

#if TARGET_OS_POSIX
/*
 *  POSIX Implementation
 */
# include <pthread.h>
class afl::sys::Mutex::Impl {
 public:
    Impl()
        {
            pthread_mutexattr_t attr;
            pthread_mutexattr_init(&attr);
            pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
            pthread_mutex_init(&m_mutex, &attr);
            pthread_mutexattr_destroy(&attr);
        }
    ~Impl()
        {
            pthread_mutex_destroy(&m_mutex);
        }
    void wait()
        {
            pthread_mutex_lock(&m_mutex);
        }
    void post()
        {
            pthread_mutex_unlock(&m_mutex);
        }

 private:
    pthread_mutex_t m_mutex;
};
#elif TARGET_OS_WIN32
/*
 *  Win32 Implementation
 */
# include <windows.h>
class afl::sys::Mutex::Impl {
 public:
    Impl()
        {
            InitializeCriticalSection(&m_criticalSection);
        }
    ~Impl()
        {
            DeleteCriticalSection(&m_criticalSection);
        }
    void wait()
        {
            EnterCriticalSection(&m_criticalSection);
        }
    void post()
        {
            LeaveCriticalSection(&m_criticalSection);
        }

 private:
    CRITICAL_SECTION m_criticalSection;
};
#else
# error Teach me about mutexes
#endif

#endif
