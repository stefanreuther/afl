/**
  *  \file afl/sys/thread.hpp
  *  \brief class afl::sys::Thread
  *
  *  \todo Set thread attributes (priority, stack size)
  */
#ifndef AFL_AFL_SYS_THREAD_HPP
#define AFL_AFL_SYS_THREAD_HPP

#include "afl/base/uncopyable.hpp"
#include "afl/sys/mutex.hpp"
#include "afl/base/runnable.hpp"
#include "afl/string/string.hpp"
#include "afl/sys/types.hpp"

namespace afl { namespace sys {

    /** Thread.
        Implements a handle to a thread (separate path of execution).
        The thread can run once during the lifetime of the Thread object.

        A thread can have one of three states:
        - NotStarted (object has just been constructed)
        - Started (...and start() has been called)
        - Finished (...and join() has been called) */
    class Thread : public afl::base::Uncopyable {
     public:
        /** Constructor.
            \param name Name of thread
            \param r Code to execute in the thread */
        Thread(const char* name, afl::base::Runnable& r);

        /** Constructor.
            \param name Name of thread
            \param r Code to execute in the thread */
        Thread(const String_t& name, afl::base::Runnable& r);

        /** Destructor.
            If the thread was started, waits for it to complete. */
        ~Thread();

        /** Start the thread.
            Parallel execution will begin by calling the thread's Runnable's run() method.
            This method may be called only once, and only before join() has been called. */
        void start();

        /** Wait for the thread to finish.
            Suspends the current thread until the thread's Runnable's run() method terminates.
            If start() has not been called yet, returns immediately. */
        void join();

        /** Get name of thread.
            Returns the name give to the thread at creation.
            \return Name */
        const String_t& getName() const;

        /** Sleep.
            Defers the current thread's execution by #time milliseconds.

            Note that various events can cause the thread to sleep longer (busy CPU)
            or shorter (rounding, interrupts);
            if required, check the actual sleep duration using afl::sys::Time::getTickCounter().

            Also note that this function sleeps the currently executing thread,
            not the thread behind the Thread object.
            It just happens that sleep() traditionally is part of Thread.

            \param time Time in milliseconds to wait */
        static void sleep(Timeout_t time);

     private:
        enum State {
            NotStarted,
            Started,
            Finished
        };
        Mutex m_stateMutex;
        State m_state;
        afl::base::Runnable& m_runnable;

        String_t m_name;

        class Impl;
        friend class Impl;
        Impl* m_pImpl;
    };

} }

inline const String_t&
afl::sys::Thread::getName() const
{
    return m_name;
}

#endif
