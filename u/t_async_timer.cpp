/**
  *  \file u/t_async_timer.cpp
  *  \brief Test for afl::async::Timer
  */

#include "afl/async/timer.hpp"

#include "u/t_async.hpp"
#include "afl/async/operation.hpp"
#include "afl/async/controller.hpp"
#include "afl/sys/thread.hpp"

/** Simple test. */
void
TestAsyncTimer::testIt()
{
    afl::async::Timer t;
    afl::async::Controller ctl;
    afl::async::Operation op;

    // Timer is idle; must not fire.
    t.waitAsync(ctl, op);
    TS_ASSERT(ctl.wait(100) == 0);

    // Make a one-shot timer.
    t.start(100);
    TS_ASSERT(ctl.wait(200) == &op);
    t.waitAsync(ctl, op);
    TS_ASSERT(ctl.wait(100) == 0);

    // Make an auto-repeat timer.
    t.start(100, true);
    TS_ASSERT(ctl.wait(200) == &op);
    t.waitAsync(ctl, op);
    TS_ASSERT(ctl.wait(200) == &op);
    t.waitAsync(ctl, op);
    TS_ASSERT(ctl.wait(200) == &op);
    t.waitAsync(ctl, op);

    // Stop it.
    t.stop();
    TS_ASSERT(ctl.wait(100) == 0);

    // Make auto-repeat and let it fire often.
    t.start(100, true);
    afl::sys::Thread::sleep(500);
    TS_ASSERT(ctl.wait(0) == &op);
    t.waitAsync(ctl, op);
    TS_ASSERT(ctl.wait(0) == &op);
    t.waitAsync(ctl, op);
    TS_ASSERT(ctl.wait(0) == &op);
}
