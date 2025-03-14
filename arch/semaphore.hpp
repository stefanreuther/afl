/**
  *  \file arch/semaphore.hpp
  *  \brief System-dependant Part of afl/sys/semaphore.cpp
  */
#ifndef AFL_ARCH_SEMAPHORE_HPP
#define AFL_ARCH_SEMAPHORE_HPP

#include "afl/sys/semaphore.hpp"

#if TARGET_OS_POSIX
/*
 *  POSIX
 */
# include <semaphore.h>
# include <time.h>
class afl::sys::Semaphore::Impl {
 public:
    Impl(int value)
        {
            sem_init(&m_sem, 0, static_cast<unsigned int>(value));
        }

    ~Impl()
        {
            sem_destroy(&m_sem);
        }

    void post()
        {
            sem_post(&m_sem);
        }

    void wait()
        {
            sem_wait(&m_sem);
        }

    bool wait(Timeout_t t)
        {
            if (t == 0) {
                return sem_trywait(&m_sem) == 0;
            } else if (t == INFINITE_TIMEOUT) {
                wait();
                return true;
            } else {
                // Obtain the time (zero-initialize so it is not uninitialized if anything fails)
                struct timespec ts;
                ts.tv_sec = 0;
                ts.tv_nsec = 0;
                clock_gettime(CLOCK_REALTIME, &ts);

                // Add timeout. sem_timedwait wants an absolute timeout. D'ooh.
                ts.tv_sec  += t / 1000;
                ts.tv_nsec += (t % 1000) * 1000000L;
                if (ts.tv_nsec > 1000000000L) {
                    ts.tv_nsec -= 1000000000L;
                    ++ts.tv_sec;
                }

                // Do it.
                return sem_timedwait(&m_sem, &ts) == 0;
            }
        }

 private:
    sem_t m_sem;
};
#elif TARGET_OS_WIN32
/*
 *  Win32
 */
# include <windows.h>
class afl::sys::Semaphore::Impl {
 public:
    Impl(int value)
        : m_hSem(CreateSemaphore(0, value, 0x7FFFFFFF, 0))
        {
            // FIXME: handle error return
        }

    ~Impl()
        {
            CloseHandle(m_hSem);
        }

    void post()
        {
            ReleaseSemaphore(m_hSem, 1, 0);
        }

    void wait()
        {
            WaitForSingleObject(m_hSem, INFINITE);
        }

    bool wait(Timeout_t t)
        {
            DWORD timeout = (t == INFINITE_TIMEOUT ? INFINITE : t);
            return WaitForSingleObject(m_hSem, timeout) == WAIT_OBJECT_0;
        }

 private:
    HANDLE m_hSem;
};

#else
# error Teach me about semaphores
#endif

#endif
