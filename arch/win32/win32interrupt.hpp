/**
  *  \file arch/win32/win32interrupt.hpp
  *  \brief Class arch::win32::Win32Interrupt
  */
#ifndef AFL_ARCH_WIN32_WIN32INTERRUPT_HPP
#define AFL_ARCH_WIN32_WIN32INTERRUPT_HPP

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <list>
#include "afl/async/interrupt.hpp"
#include "afl/async/interruptoperation.hpp"
#include "afl/sys/mutex.hpp"
#include "arch/win32/waitrequest.hpp"

namespace arch { namespace win32 {

    /** Implementation of Interrupt for Win32. */
    class Win32Interrupt : public afl::async::Interrupt, private WaitRequest {
     public:
        /** Shortcut. */
        typedef afl::async::InterruptOperation InterruptOperation_t;

        class Instance;

        /** Constructor. */
        Win32Interrupt();

        /** Destructor. */
        ~Win32Interrupt();

        // Interrupt:
        virtual afl::async::InterruptOperation::Kinds_t wait(afl::async::Controller& ctl, afl::async::InterruptOperation::Kinds_t kinds, afl::sys::Timeout_t timeout);
        virtual void waitAsync(afl::async::Controller& ctl, afl::async::InterruptOperation& op);
        virtual void cancel(afl::async::Controller& ctl, afl::async::Operation& op);

        bool handleInterrupt(InterruptOperation_t::Kind k);

     private:
        std::list<InterruptOperation_t*> m_pending;
        afl::sys::Mutex m_mutex;

        static const size_t N = 3;
        LONG m_attached[N];
        LONG m_signalled[N];
        HANDLE m_event;

        bool handleWaitReady();

        void setupOnce(InterruptOperation_t& op, InterruptOperation_t::Kind k);
    };

} }

#endif
