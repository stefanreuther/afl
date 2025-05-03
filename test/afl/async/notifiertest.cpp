/**
  *  \file test/afl/async/notifiertest.cpp
  *  \brief Test for afl::async::Notifier
  */

#include "afl/async/notifier.hpp"

#include "afl/async/controller.hpp"
#include "afl/async/operation.hpp"
#include "afl/async/semaphore.hpp"
#include "afl/sys/semaphore.hpp"
#include "afl/test/testrunner.hpp"


/** Test simple default use-cases of Notifier. */
AFL_TEST("afl.async.Notifier:simple", a)
{
    // Get default notifier
    afl::async::Notifier& defaultNotifier =
        afl::async::Notifier::getDefaultInstance();

    // Make a controller and post an operation; check that we get it.
    afl::async::Controller ctl;
    afl::async::Operation op;
    op.setController(&ctl);
    defaultNotifier.notify(op);
    a.check("01", ctl.wait(0) == &op);
    a.check("02", ctl.wait(0) == 0);
}

/** Test using a nondefault notifier. */
AFL_TEST("afl.async.Notifier:integration", a)
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
    a.check("01", ctl.wait(100) == 0);

    // Raise the semaphore.
    semaphore.post();

    // First check that our notifier got its semaphore (but just once)
    a.check("11", notifier.m_semaphore.wait(100));
    a.check("12", !notifier.m_semaphore.wait(100));

    // Must also actually arrive on the controller
    a.check("13", ctl.wait(100) == &op);
    a.check("14", ctl.wait(100) == 0);
}
