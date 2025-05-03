/**
  *  \file afl/async/controller.hpp
  *  \brief Class afl::async::Controller
  */
#ifndef AFL_AFL_ASYNC_CONTROLLER_HPP
#define AFL_AFL_ASYNC_CONTROLLER_HPP

#include "afl/base/uncopyable.hpp"
#include "afl/sys/types.hpp"
#include "afl/async/operationlist.hpp"
#include "afl/sys/mutex.hpp"

namespace afl { namespace async {

    class Operation;

    /** Asynchronous operation controller.
        An object of this type coordinates multiple asynchronous operations.
        Each thread that wants to perform asynchronous operations must use one Controller;
        Controllers must not be shared between threads.
        If absolutely required, a thread can use multiple controllers,
        but a Controller might be too expensive to construct in a tight loop.

        Asynchronous operations are typically started on an object derived from Cancelable.
        They receive parameters through a Operation descendant.
        In addition, they take a Controller as parameter.

        Synchronous operations typically operate similar to asynchronous ones,
        and also take a Controller and Operation.
        This is required to guarantee some sort of ordering,
        i.e. synchronous and asynchronous operations on the same object complete in the order as given
        and do not overtake each other.

        The actual work may or may not immediately start to happen truly asynchronously.
        When Controller::wait() is called, the thread will wait for an asynchronous operation to complete. */
    class Controller : public afl::base::Uncopyable {
     public:
        class Impl;

        /** Constructor. */
        Controller();

        /** Destructor. */
        ~Controller();

        /** Wait for any operation completion.
            \param timeout Timeout (in milliseconds) to wait.
            Can also be 0 (just check) or INFINITE_TIMEOUT (wait indefinitely).
            \return A completed operation; null if the timeout was reached.
            When INFINITE_TIMEOUT, this never returns null. */
        Operation* wait(afl::sys::Timeout_t timeout = afl::sys::INFINITE_TIMEOUT);

        /** Wait for a specific operation completion.
            This function is used in the implementation of synchronous operations.
            \param op Operation to wait for
            \param timeout Timeout (in milliseconds) to wait.
            Can also be 0 (just check) or INFINITE_TIMEOUT (wait indefinitely).
            \retval true Operation completed
            \retval false Timeout was reached */
        bool wait(Operation& op, afl::sys::Timeout_t timeout = afl::sys::INFINITE_TIMEOUT);

        /** Post a result.
            Can be called from any thread.
            If the thread that owns this Controller currently is in wait(), that will return op.
            Otherwise, the operation will be queued for later return by wait().
            \param op Operation */
        void post(Operation& op);

        /** Revert a previous post().
            Removes the operation from the queue, so it will no longer be returned by wait().
            Use this for implementations of cancel().
            \param op Operation */
        void revertPost(Operation& op);

        /** Access implementation.
            \return Reference to internal implementation.
            Note that you need a platform header file to make use of this.
            \internal */
        Impl& getImplementation();

     private:
        afl::sys::Mutex m_completedMutex;
        OperationList<Operation> m_completedOperations;

        Impl* m_pImpl;
    };

} }

inline afl::async::Controller::Impl&
afl::async::Controller::getImplementation()
{
    return *m_pImpl;
}

#endif
