/**
  *  \file afl/async/timer.cpp
  *  \brief Class afl::async::Timer
  */

#include <list>
#include "afl/async/timer.hpp"
#include "afl/async/controller.hpp"
#include "afl/async/notifier.hpp"
#include "afl/base/stoppable.hpp"
#include "afl/sys/mutexguard.hpp"
#include "afl/sys/semaphore.hpp"
#include "afl/sys/thread.hpp"
#include "afl/sys/time.hpp"

/******************************** Manager ********************************/

/** Timer manager.
    This thread is auto-created when first needed.

    Mutex usage: Manager calls Timer methods with its mutex held.
    Therefore, Timer must not call Manager methods with its own mutex held.

    \todo Would it make sense to shut down the thread after being idle for a given time?
    This would make starting it a little more complicated.
    \internal */
class afl::async::Timer::Manager : public afl::base::Stoppable {
 public:
    /** Constructor. */
    Manager()
        : m_mutex(),
          m_semaphore(0),
          m_timers(),
          m_terminating(false),
          m_thread("AsyncTimer", *this)
        {
            m_thread.start();
        }

    /** Destructor. */
    ~Manager()
        { }

    /** Add a timer.
        \param t the timer */
    void add(Timer& t)
        {
            afl::sys::MutexGuard g(m_mutex);
            m_timers.push_back(&t);
        }

    /** Remove a timer.
        \param t the timer */
    void remove(Timer& t)
        {
            afl::sys::MutexGuard g(m_mutex);
            m_timers.remove(&t);
        }

    /** Trigger update. Causes the timer thread to reconsider timers because something changed. */
    void poke()
        {
            m_semaphore.post();
        }

    /** Main loop. */
    virtual void run()
        {
            while (!m_terminating) {
                m_semaphore.wait(update(afl::sys::Time::getTickCounter()));
            }
        }

    virtual void stop()
        {
            m_terminating = true;
            m_semaphore.post();
        }

    /** Get global timer manager instance. */
    static Manager& getInstance()
        {
            static Manager mgr;
            return mgr;
        }

 private:
    afl::sys::Mutex m_mutex;
    afl::sys::Semaphore m_semaphore;
    std::list<Timer*> m_timers;
    bool m_terminating;

    // Thread. Must be last so it dies first, and all other properties are still present.
    afl::sys::Thread m_thread;

    /** Update all timers.
        \param now Current tick counter
        \return New timeout */
    afl::sys::Timeout_t update(uint32_t now)
        {
            afl::sys::MutexGuard g(m_mutex);
            afl::sys::Timeout_t result = afl::sys::INFINITE_TIMEOUT;
            for (std::list<Timer*>::iterator i = m_timers.begin(); i != m_timers.end(); ++i) {
                uint32_t n = (*i)->update(now);
                if (n < result) {
                    result = n;
                }
            }
            return result;
        }
};


/********************************* Timer *********************************/

afl::async::Timer::Timer()
    : m_manager(Manager::getInstance()),
      m_mutex(),
      m_waiters(),
      m_numSignals(0),
      m_interval(0),
      m_active(false),
      m_cyclic(false),
      m_lastCheck(0)
{
    m_manager.add(*this);
}

afl::async::Timer::~Timer()
{
    m_manager.remove(*this);
}

void
afl::async::Timer::start(afl::sys::Timeout_t interval, bool cyclic)
{
    {
        afl::sys::MutexGuard g(m_mutex);
        m_interval = interval;
        m_active = true;
        m_cyclic = cyclic;
        m_lastCheck = afl::sys::Time::getTickCounter();
    }
    m_manager.poke();
}

void
afl::async::Timer::stop()
{
    {
        afl::sys::MutexGuard g(m_mutex);
        m_interval = 0;
        m_active = false;
        m_cyclic = false;
        m_numSignals = 0;
    }
    m_manager.poke();
}

bool
afl::async::Timer::wait(Controller& ctl, afl::sys::Timeout_t timeout)
{
    Operation op;
    waitAsync(ctl, op);
    if (!ctl.wait(op, timeout)) {
        cancel(ctl, op);
        return false;
    } else {
        return true;
    }
}

void
afl::async::Timer::waitAsync(Controller& ctl, Operation& op)
{
    afl::sys::MutexGuard g(m_mutex);
    op.setController(&ctl);
    if (m_numSignals == 0) {
        // Must wait
        m_waiters.pushBack(&op);
    } else {
        // We have a pending signal
        --m_numSignals;
        op.getNotifier().notifyDirect(op);
    }
}

void
afl::async::Timer::cancel(Controller& ctl, Operation& op)
{
    afl::sys::MutexGuard g(m_mutex);
    m_waiters.remove(&op);
    ctl.revertPost(op);
}


afl::sys::Timeout_t
afl::async::Timer::update(uint32_t now)
{
    afl::sys::MutexGuard g(m_mutex);
    if (!m_active) {
        return afl::sys::INFINITE_TIMEOUT;
    } else {
        // Check elapsed time
        uint32_t elapsed = now - m_lastCheck;
        if (elapsed >= m_interval) {
            // Timer fires
            if (Operation* op = m_waiters.extractFront()) {
                op->getNotifier().notify(*op);
            } else {
                ++m_numSignals;
            }

            // Update status
            m_active = m_cyclic;
            m_lastCheck += m_interval;
            elapsed -= m_interval;
        }

        // Compute next firing time
        if (elapsed >= m_interval) {
            // Timer would have to immediately fire again. Throttle it.
            m_lastCheck = now - m_interval + 1;
            return 1;
        } else {
            // Schedule next event
            return m_interval - elapsed;
        }
    }
}
