/**
  *  \file u/t_async_notifier.cpp
  *  \brief Test for afl::async::Notifier
  */

#include "afl/async/notifier.hpp"

#include "u/t_async.hpp"
#include "afl/async/controller.hpp"
#include "afl/async/operation.hpp"
#include "afl/sys/semaphore.hpp"
#include "afl/async/semaphore.hpp"

/** Test simple default use-cases of Notifier. */
void
TestAsyncNotifier::testSimple()
{
    // Get default notifier
    afl::async::Notifier& defaultNotifier =
        afl::async::Notifier::getDefaultInstance();

    // Make a controller and post an operation; check that we get it.
    afl::async::Controller ctl;
    afl::async::Operation op;
    op.setController(&ctl);
    defaultNotifier.notify(op);
    TS_ASSERT(ctl.wait(0) == &op);
    TS_ASSERT(ctl.wait(0) == 0);

    // Same things for notifyDirect
    defaultNotifier.notifyDirect(op);
    TS_ASSERT(ctl.wait(0) == &op);
    TS_ASSERT(ctl.wait(0) == 0);
}

/** Test using a nondefault notifier. */
void
TestAsyncNotifier::testIntegration()
{
    class MyNotifier : public afl::async::Notifier {
     public:
        MyNotifier()
            : m_semaphore(0)
            { }

        void notify(afl::async::Operation& op)
            {
                m_semaphore.post();
                op.getController()->post(op);
            }

        void notifyDirect(afl::async::Operation& op)
            {
                notify(op);
            }

        afl::sys::Semaphore m_semaphore;
    };

    // Asynchronously wait for an async semaphore using the custom notifier
    MyNotifier notifier;
    afl::async::Semaphore semaphore;
    afl::async::Operation op;
    afl::async::Controller ctl;
    op.setNotifier(notifier);
    semaphore.waitAsync(ctl, op);

    // Nobody raised the semaphore yet
    TS_ASSERT(ctl.wait(100) == 0);

    // Raise the semaphore.
    semaphore.post();

    // First check that our notifier got its semaphore (but just once)
    TS_ASSERT(notifier.m_semaphore.wait(100));
    TS_ASSERT(!notifier.m_semaphore.wait(100));

    // Must also actually arrive on the controller
    TS_ASSERT(ctl.wait(100) == &op);
    TS_ASSERT(ctl.wait(100) == 0);
}
