/**
  *  \file arch/posix/posixinterrupt.hpp
  *  \brief Class arch::posix::PosixInterrupt
  */
#ifndef AFL_ARCH_POSIX_POSIXINTERRUPT_HPP
#define AFL_ARCH_POSIX_POSIXINTERRUPT_HPP

#include <list>
#include <memory>
#include "afl/async/interrupt.hpp"
#include "afl/async/interruptoperation.hpp"
#include "afl/sys/mutex.hpp"

namespace arch { namespace posix {

    /** Implementation of Interrupt for POSIX.

        The basic idea is:

        (a) When an interrupt is first requested, the corresponding signal handler is installed and an associated Instance object is created.
        That object contains a pipe; the signal handler will write a byte into that pipe.

        (b) Whenever a wait operation is active for a set of interrupts, we will have the corresponding Instances wait for a byte in the pipe
        using the given controllers. That is, if three threads wait for a set of signals, all three will wait for the byte.
        If a byte is received, one of these threads will receive it, and will cause all listeners to be notified,
        canceling the other waiters (Instance) in the process. */
    class PosixInterrupt : public afl::async::Interrupt {
     public:
        /** Shortcut. */
        typedef afl::async::InterruptOperation InterruptOperation_t;

        class Instance;

        /** Constructor. */
        PosixInterrupt();

        /** Destructor. */
        ~PosixInterrupt();

        // Interrupt:
        virtual afl::async::InterruptOperation::Kinds_t wait(afl::async::Controller& ctl, afl::async::InterruptOperation::Kinds_t kinds, afl::sys::Timeout_t timeout);
        virtual void waitAsync(afl::async::Controller& ctl, afl::async::InterruptOperation& op);
        virtual void cancel(afl::async::Controller& ctl, afl::async::Operation& op);

        /** Notify interrupt.
            Called from a Controller's notification callback.
            \param k Interrupt kind */
        void notifyInterrupt(afl::async::InterruptOperation::Kind k);

        /** Handle signal.
            Called from signal handler.
            \param n Signal number */
        void handleSignal(int n);

     private:
        std::auto_ptr<Instance> m_int;
        std::auto_ptr<Instance> m_hup;
        std::auto_ptr<Instance> m_term;

        std::list<InterruptOperation_t*> m_pending;

        afl::sys::Mutex m_mutex;

        void cancelOnce(std::auto_ptr<Instance>& p, InterruptOperation_t::Kind k, InterruptOperation_t& op);
        void waitOnce(std::auto_ptr<Instance>& p, InterruptOperation_t::Kind k, int signalNumber, afl::async::Controller& ctl, InterruptOperation_t& op);
    };

} }

#endif
