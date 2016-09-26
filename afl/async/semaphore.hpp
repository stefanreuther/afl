/**
  *  \file afl/async/semaphore.hpp
  *  \brief Class afl::async::Semaphore
  */
#ifndef AFL_AFL_ASYNC_SEMAPHORE_HPP
#define AFL_AFL_ASYNC_SEMAPHORE_HPP

#include "afl/sys/mutex.hpp"
#include "afl/async/operationlist.hpp"
#include "afl/sys/types.hpp"
#include "afl/async/cancelable.hpp"
#include "afl/async/synchronisationobject.hpp"

namespace afl { namespace async {

    class Operation;
    class Controller;

    /** Asynchronous semaphore.
        Implements a regular counting semaphore within the async framework.

        The semaphore has a numerical value.
        - wait() decreases the value; if it reaches 0, waits.
        - post() increases the value; if someone waits, wakes them. */
    class Semaphore : public SynchronisationObject {
     public:
        /** Constructor.
            \param initialValue Initial value of the semaphore */
        explicit Semaphore(uint32_t initialValue = 0);

        /** Destructor. */
        ~Semaphore();

        /** Increase semaphore value by one.
            If the semaphore's value is zero, and another thread is waiting in wait(),
            releases that thread. */
        void post();

        /** Decrease semaphore value by one, synchronous.
            \param ctl Controller
            \param timeout Timeout in milliseconds
            \retval true Semaphore obtained
            \retval false Timeout reached */
        bool wait(Controller& ctl, afl::sys::Timeout_t timeout = afl::sys::INFINITE_TIMEOUT);

        /** Decrease semaphore value by one, asynchronous.
            \param ctl Controller
            \param op Operation. ctl.wait() returns the operation when the semaphore was obtained. */
        void waitAsync(Controller& ctl, Operation& op);

        /** Cancel a pending waitAsync().
            \param ctl Controller
            \param op Operation */
        void cancel(Controller& ctl, Operation& op);

     private:
        afl::sys::Mutex m_mutex;
        OperationList<Operation> m_waiters;
        uint32_t m_value;
    };

} }

#endif
