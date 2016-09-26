/**
  *  \file afl/sys/mutex.hpp
  *  \brief Class afl::sys::Mutex
  */
#ifndef AFL_AFL_SYS_MUTEX_HPP
#define AFL_AFL_SYS_MUTEX_HPP

#include "afl/base/uncopyable.hpp"

namespace afl { namespace sys {

    /** Mutex.
        A standard, process-local, recursive (nestable) mutex. */
    class Mutex : afl::base::Uncopyable {
     public:
        /** Constructor. Make a mutex. */
        Mutex();

        /** Destructor. Destroy the mutex. */
        ~Mutex();

        /** Obtain the resource ("lock").
            Blocks if the resource is already locked by another thread, until it becomes available.

            If your thread already owns the resource, wait() succeeds.
            You must then call post() an additional time to release it. */
        void wait();

        /** Release the resource ("unlock"). */
        void post();

     private:
        class Impl;
        Impl* m_pImpl;
    };

} }

#endif
