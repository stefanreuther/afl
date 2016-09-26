/**
  *  \file afl/async/synchronisationobject.hpp
  *  \brief Interface afl::async::SynchronisationObject
  */
#ifndef AFL_AFL_ASYNC_SYNCHRONISATIONOBJECT_HPP
#define AFL_AFL_ASYNC_SYNCHRONISATIONOBJECT_HPP

#include "afl/async/cancelable.hpp"
#include "afl/sys/types.hpp"

namespace afl { namespace async {

    /** Interface for a synchronisation object.
        A synchronisation object receives data-less notifications.

        Special care must be taken when using timeouts or cancel().
        If an operation is aborted mid-way, there is no way to obtain the status of the wait.
        The synchronisation event might already have occured by the time the thread decided to abort the wait.
        Therefore, in the general case, use timeouts only to detect connection loss or hang,
        not as part of a regular protocol. */
    class SynchronisationObject : public Cancelable {
     public:
        /** Wait for synchronisation event, synchronous.
            This blocks the current thread until the event occured or the timeout expires.
            Implementations are encouraged to implement special handling for timeout=0 and timeout=INFINITE_TIMEOUT.
            This function does not take an Operation parameter because there is no data to transfer.
            \param ctl Controller
            \param timeout Timeout in milliseconds
            \retval true Synchronisation event received
            \retval false Timeout reached */
        virtual bool wait(Controller& ctl, afl::sys::Timeout_t timeout = afl::sys::INFINITE_TIMEOUT) = 0;

        /** Wait for synchronisation event, asynchronous.
            This call completes without significant blocking.
            The actual wait will be performed in the background,
            possibly when Controller::wait() is called.
            \param ctl Controller
            \param op Operation. ctl.wait() returns the operation when the semaphore was obtained. */
        virtual void waitAsync(Controller& ctl, Operation& op) = 0;

        /** Cancel a pending waitAsync().
            This function must, like all functions that access a Controller,
            be called from the thread that owns the Controller.
            \param ctl Controller
            \param op Operation */
        virtual void cancel(Controller& ctl, Operation& op) = 0;
    };

} }

#endif
