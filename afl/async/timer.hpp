/**
  *  \file afl/async/timer.hpp
  *  \brief Class afl::async::Timer
  */
#ifndef AFL_AFL_ASYNC_TIMER_HPP
#define AFL_AFL_ASYNC_TIMER_HPP

#include "afl/sys/types.hpp"
#include "afl/sys/mutex.hpp"
#include "afl/async/operationlist.hpp"
#include "afl/async/synchronisationobject.hpp"

namespace afl { namespace async {

    class Operation;
    class Controller;

    /** Timer.
        This timer generates synchronisation events, either once or repeatedly.
        It can be reconfigured at any time.

        A Timer somehow behaves like a Semaphore that automatically increases according to the configured behaviour:
        If the timer fires, but no wait() is active, that notification will be registered
        and be immediately given to the next wait() call.

        Timers use a background thread to actually implement the timing behaviour. */
    class Timer : public SynchronisationObject {
     public:
        /** Constructor.
            Makes an idle timer. */
        Timer();

        /** Destructor. */
        ~Timer();

        /** Start the timer.
            The new configuration overwrites the existing one.
            The new timer starts running immediately.
            Previous unreceived notifications remain pending.
            \param interval Timer interval
            \param cyclic true to make a cyclic timer, false to make a one-shot timer */
        void start(afl::sys::Timeout_t interval, bool cyclic = false);

        /** Stop the timer.
            No more notifications will be generated.
            Previous unreceived notifications will be taken back.
            However, notifications already given to the Controller will remain there; use cancel() to get rid of those. */
        void stop();

        bool wait(Controller& ctl, afl::sys::Timeout_t timeout = afl::sys::INFINITE_TIMEOUT);

        void waitAsync(Controller& ctl, Operation& op);

        void cancel(Controller& ctl, Operation& op);

     private:
        class Manager;
        friend class Manager;

        // Async
        Manager& m_manager;
        afl::sys::Mutex m_mutex;
        OperationList<Operation> m_waiters;
        uint32_t m_numSignals;

        // Configuration
        afl::sys::Timeout_t m_interval;
        bool m_active;
        bool m_cyclic;

        // Status
        uint32_t m_lastCheck;
        afl::sys::Timeout_t update(uint32_t now);
    };

} }

#endif
