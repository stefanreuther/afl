/**
  *  \file afl/async/interrupt.hpp
  *  \brief Interface afl::async::Interrupt
  */
#ifndef AFL_AFL_ASYNC_INTERRUPT_HPP
#define AFL_AFL_ASYNC_INTERRUPT_HPP

#include "afl/async/operation.hpp"
#include "afl/bits/smallset.hpp"
#include "afl/async/cancelable.hpp"
#include "afl/sys/types.hpp"
#include "afl/async/interruptoperation.hpp"

namespace afl { namespace async {

    class Controller;

    /** Receiving interrupts from the operating system.
        An Interrupt instance allows you to receive
        - signals (POSIX)
        - console control events (Win32)
        from the operating system.
        The available set of events is defined in InterruptOperation.
        There exists a process-wide instance of Interrupt that is accessible using getInstance().
        This instance can be used as often as required.

        <b>General Notes</b>

        Waiting for an interrupt for the first time will tell the operating system that you handle it, and thus turn off the default handling.
        If you handle an interrupt, you must therefore restart your wait to keep receiving.

        The interface allows for receiving multiple interrupts at once at the implementation's discretion.

        <b>Threads</b>

        Interrupt can be used from multiple threads.
        If an interrupt happens, all waiting threads will be notified.
        Note, however, that if multiple interrupts of the same kind appear in quick succession, it is not guaranteed that all threads receive the notification,
        because some may not have restarted their wait() yet. */
    class Interrupt : public Cancelable {
     public:
        /** Synchronous wait.
            \param ctl Controller
            \param kinds Interrupts to wait for
            \param timeout Timeout
            \return Set of received interrupts. Empty if the timeout occured */
        virtual InterruptOperation::Kinds_t wait(Controller& ctl, InterruptOperation::Kinds_t kinds, afl::sys::Timeout_t timeout = afl::sys::INFINITE_TIMEOUT) = 0;

        /** Asynchronous wait.
            \param ctl Controller
            \param op Wait operation (should have nonempty getRequested()). ctl.wait() will return the operation when an interrupt happened */
        virtual void waitAsync(Controller& ctl, InterruptOperation& op) = 0;

        /** Cancel asynchronous wait.
            \param ctl Controller
            \param op Wait operation */
        virtual void cancel(Controller& ctl, Operation& op) = 0;

        /** Get system instance.
            \return instance */
        static Interrupt& getInstance();
    };

} }

#endif
