/**
  *  \file afl/async/mutex.hpp
  *  \brief Class afl::async::Mutex
  */
#ifndef AFL_AFL_ASYNC_MUTEX_HPP
#define AFL_AFL_ASYNC_MUTEX_HPP

#include "afl/sys/mutex.hpp"
#include "afl/async/operationlist.hpp"
#include "afl/sys/types.hpp"
#include "afl/async/synchronisationobject.hpp"

namespace afl { namespace async {

    class Operation;
    class Controller;

    /** Asynchronous mutex.
        Implements a regular recursive mutex within the async framework.

        Only one thread at a time can own the mutex.
        A thread is identified by its Controller.
        A thread can enter the mutex multiple times, and in this case must return it multiple times.

        - wait() enters the mutex
        - post() releases the mutex

        If you acquire the mutex in the instant you decide to cancel() a wait or the timeout happens,
        the mutex will be registered as owned by you.
        You can therefore call post() after cancel() or an unsuccessful wait()
        to get the mutex back into a working state.
        post() will not do anything if you don't actually own the mutex. */
    class Mutex : public SynchronisationObject {
     public:
        /** Constructor. */
        Mutex();

        /** Destructor. */
        ~Mutex();

        /** Obtain the resource.
            \param ctl Controller
            \param timeout Timeout in milliseconds
            \retval true Resource obtained
            \retval false Timeout reached */
        bool wait(Controller& ctl, afl::sys::Timeout_t timeout = afl::sys::INFINITE_TIMEOUT);

        /** Obtain the resource, asynchronous.
            \param ctl Controller
            \param op Operation. ctl.wait() returns the operation when the resource was obtained. */
        void waitAsync(Controller& ctl, Operation& op);

        /** Release the resource.
            If another thread (=another Controller) is waiting on it, signals that.

            This function should normally only be called when the current thread (=ctl) actually owns the mutex.
            If it does not own the mutex, the call will be silently ignored.
            \param ctl Controller */
        void post(Controller& ctl);

        /** Cancel a pending lockAsync().
            \param ctl Controller
            \param op Operation */
        void cancel(Controller& ctl, Operation& op);

     private:
        afl::sys::Mutex m_mutex;
        OperationList<Operation> m_waiters;
        uint32_t m_nestingLevel;
        Controller* m_pOwner;
    };

} }

#endif
