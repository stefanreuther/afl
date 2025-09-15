/**
  *  \file afl/async/internalinterrupt.hpp
  *  \brief Class afl::async::InternalInterrupt
  */
#ifndef AFL_AFL_ASYNC_INTERNALINTERRUPT_HPP
#define AFL_AFL_ASYNC_INTERNALINTERRUPT_HPP

#include <list>
#include "afl/async/interrupt.hpp"
#include "afl/async/interruptoperation.hpp"
#include "afl/sys/mutex.hpp"

namespace afl { namespace async {

    class Controller;

    /** Internal implementation of Interrupt.
        Provides a fully local, operating-system independant implementation of the Interrupt interface.
        The receiver of interrupt signals can use it normally;
        another thread can post interrupts using post().
        While this implementation is intended for testing, it might also have production uses. */
    class InternalInterrupt : public Interrupt {
     public:
        /** Constructor. */
        InternalInterrupt();

        /** Destructor. */
        ~InternalInterrupt();

        // Interrupt:
        virtual InterruptOperation::Kinds_t wait(Controller& ctl, InterruptOperation::Kinds_t kinds, afl::sys::Timeout_t timeout = afl::sys::INFINITE_TIMEOUT);
        virtual void waitAsync(Controller& ctl, InterruptOperation& op);
        virtual void cancel(Controller& ctl, Operation& op);

        /** Post interrupts.
            If any component is currently waiting on any of the affected interrupts, it will be notified.
            If no component is currently waiting, nothing happens; the interrupt is not queued.

            @param kinds Interrupts to signal */
        void post(InterruptOperation::Kinds_t kinds);

     private:
        afl::sys::Mutex m_mutex;
        std::list<InterruptOperation*> m_waiters;
    };

} }

#endif
