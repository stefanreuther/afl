/**
  *  \file test/afl/async/internalinterrupttest.cpp
  *  \brief Test for afl::async::InternalInterrupt
  */

#include "afl/async/internalinterrupt.hpp"

#include "afl/test/testrunner.hpp"
#include "afl/async/controller.hpp"
#include "afl/async/interruptoperation.hpp"
#include "afl/sys/thread.hpp"
#include "afl/base/stoppable.hpp"

using afl::async::Controller;
using afl::async::InternalInterrupt;
using afl::async::InterruptOperation;
using afl::async::Operation;

/** Simple asynchronous operation. */
AFL_TEST("afl.async.InternalInterrupt:async", a)
{
    Controller ctl;
    InternalInterrupt testee;

    // Wait
    InterruptOperation op1(InterruptOperation::Kinds_t() + InterruptOperation::Break);
    InterruptOperation op2(InterruptOperation::Kinds_t() + InterruptOperation::Hangup + InterruptOperation::Terminate);
    testee.waitAsync(ctl, op1);
    testee.waitAsync(ctl, op2);

    // Post
    testee.post(InterruptOperation::Kinds_t() + InterruptOperation::Break + InterruptOperation::Hangup);

    // Wait
    bool one = false, two = false;
    while (Operation* p = ctl.wait(0)) {
        if (p == &op1) {
            a.check("01", !one);
            one = true;
        } else if (p == &op2) {
            a.check("02", !two);
            two = true;
        } else {
            a.check("03", false);
        }
    }

    a.check("11", one);
    a.check("12", two);
}

/** Simple synchronous operation. */
AFL_TEST("afl.async.InternalInterrupt:async", a)
{
    InternalInterrupt testee;
    Controller ctl;

    class Poster : public afl::base::Stoppable {
     public:
        Poster(InternalInterrupt& t)
            : m_int(t)
            { }
        virtual void run()
            {
                afl::sys::Thread::sleep(50);
                m_int.post(InterruptOperation::Kinds_t() + InterruptOperation::Break);
            }
        virtual void stop()
            { }
     private:
        InternalInterrupt& m_int;
    };
    Poster p(testee);
    afl::sys::Thread t("async", p);
    t.start();

    // Wait
    InterruptOperation::Kinds_t ks = testee.wait(ctl, InterruptOperation::Kinds_t(InterruptOperation::Break), 500);

    // Verify
    a.checkEqual("01", ks, InterruptOperation::Kinds_t(InterruptOperation::Break));

    t.join();
}

/** Failing synchronous operation. */
AFL_TEST("afl.async.InternalInterrupt:async:fail", a)
{
    InternalInterrupt testee;
    Controller ctl;

    InterruptOperation::Kinds_t ks = testee.wait(ctl, InterruptOperation::Kinds_t(InterruptOperation::Break), 100);
    a.checkEqual("01", ks, InterruptOperation::Kinds_t());
}
