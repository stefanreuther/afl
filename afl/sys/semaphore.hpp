/**
  *  \file afl/sys/semaphore.hpp
  *  \brief Class afl::sys::Semaphore
  */
#ifndef AFL_AFL_SYS_SEMAPHORE_HPP
#define AFL_AFL_SYS_SEMAPHORE_HPP

#include "afl/base/uncopyable.hpp"
#include "afl/sys/types.hpp"

namespace afl { namespace sys {

    /** Counting semaphore.
        A standard, process-local counting semaphore. */
    class Semaphore : public afl::base::Uncopyable {
     public:
        /** Constructor.
            \param value Initial value of the semaphore (must not be negative) */
        explicit Semaphore(int value);

        /** Destructor. */
        ~Semaphore();

        /** Increase semaphore value by one.
            If the semaphore's value is zero, and another thread is waiting in wait(),
            releases that thread. */
        void post();

        /** Decrease semaphore value by one.
            If the semaphore's value is zero, waits until another thread calls post(). */
        void wait();

        /** Decrease semaphore value by one, with timeout.
            If the semaphore's value is zero, waits for #timeout milliseconds that another thread calls post().
            If no other thread does so, returns false.
            The special values 0 (do not wait at all) and INFINITE_TIMEOUT (wait indefinitely, like parameterless wait())
            are allowed.
            \param timeout Time to wait
            \retval true Semaphore obtained successfully within requested time
            \retval false Semaphore could not be obtained */
        bool wait(Timeout_t timeout);

     private:
        class Impl;
        Impl* m_pImpl;
    };
    
} }

#endif
