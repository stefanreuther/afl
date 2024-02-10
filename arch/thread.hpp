/**
  *  \file arch/thread.hpp
  *  \brief System-dependant part of afl/sys/thread.cpp
  */
#ifndef AFL_ARCH_THREAD_HPP
#define AFL_ARCH_THREAD_HPP

#include "afl/sys/thread.hpp"

#if TARGET_OS_POSIX
/*
 *  POSIX
 */
#include <pthread.h>
#include <time.h>
#include "arch/posix/posix.hpp"
#include "afl/string/string.hpp"

/*
 *  Thread names in POSIX:
 *  - glibc 2.12: pthread_setname_np(pthread_t, const char*), up to 15 characters
 *  - IBM:        pthread_setname_np(pthread_t, const char*), up to 15 characters
 *  - OpenVMS:    pthread_setname_np(pthread_t, const char*, void*)
 *  - QNX:        pthread_setname_np(pthread_t, const char*)
 *  - BSD:        pthread_set_name_np(pthread_t, const char*)
 *  - MacOS:      pthread_setname_np(pthread_t), requires <pthread_np.h>
 *
 *  This is just a debugging feature so I don't want to invest too much effort in it.
 *  Let's just call pthread_setname_np in the Linux/QNX/MacOS flavours, hoping that it turns into a syscall.
 *  If the system doesn't have one, ground it using a (...) pseudo function.
 */
static inline void pthread_setname_np(...) { }

const size_t THREAD_NAME_LIMIT = 15;


class afl::sys::Thread::Impl {
 public:
    Impl(Thread* parent)
        : m_handle(),
          m_parent(parent)
        { }

    ~Impl()
        { }

    void start()
        {
            pthread_create(&m_handle, 0, entry, this);
        }

    void join()
        {
            void* result;
            pthread_join(m_handle, &result);
        }

    static void sleep(Timeout_t time)
        {
            struct timespec ts;
            ts.tv_sec  = time / 1000;
            ts.tv_nsec = (time % 1000) * 1000000L;
            nanosleep(&ts, 0);
        }

 private:
    pthread_t m_handle;
    Thread* m_parent;

    static void* entry(void* p)
        {
            // Self
            Impl* me = static_cast<Impl*>(p);

            // Set name (see above)
            {
                String_t name;
                arch::posix::convertUtf8ToExternal(name, afl::string::toMemory(me->m_parent->getName()), arch::posix::RelaxedConversion);
                if (name.size() > THREAD_NAME_LIMIT) {
                    name.erase(THREAD_NAME_LIMIT);
                }
                const char* pcName = name.c_str();

                pthread_setname_np(pthread_self(), pcName);
                pthread_setname_np(pcName);
            }

            // Start it
            static_cast<Impl*>(p)->m_parent->m_runnable.run();
            return 0;
        }
};
#elif TARGET_OS_WIN32
/*
 *  Win32
 */
#include <windows.h>
class afl::sys::Thread::Impl {
 public:
    Impl(Thread* parent)
        : m_handle(),
          m_parent(parent)
        { }

    ~Impl()
        {
            CloseHandle(m_handle);
        }

    void start()
        {
            DWORD tid;
            m_handle = CreateThread(/* lpThreadAttributes: */ 0,
                                    /* dwStackSize: */ 0,
                                    entry,
                                    this,
                                    /* dwCreationFlags: */ 0,
                                    &tid);
            // FIXME: error handling
        }

    void join()
        {
            WaitForSingleObject(m_handle, INFINITE);
        }

    static void sleep(Timeout_t time)
        {
            ::Sleep(time);
        }

 private:
    HANDLE m_handle;
    Thread* m_parent;

    static DWORD WINAPI entry(LPVOID p)
        {
            static_cast<Impl*>(p)->m_parent->m_runnable.run();
            return 0;
        }
};
#else
# error Teach me about threads
#endif

#endif
