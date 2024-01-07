/**
  *  \file test/afl/async/timertest.cpp
  *  \brief Test for afl::async::Timer
  */

#include "afl/async/timer.hpp"

#include "afl/async/controller.hpp"
#include "afl/async/operation.hpp"
#include "afl/sys/thread.hpp"
#include "afl/test/testrunner.hpp"

/** Simple test. */
AFL_TEST("afl.async.Timer", a)
{
    afl::async::Timer t;
    afl::async::Controller ctl;
    afl::async::Operation op;

    // Timer is idle; must not fire.
    t.waitAsync(ctl, op);
    a.check("01", ctl.wait(100) == 0);

    // Make a one-shot timer.
    t.start(100);
    a.check("11", ctl.wait(200) == &op);
    t.waitAsync(ctl, op);
    a.check("12", ctl.wait(100) == 0);

    // Make an auto-repeat timer.
    t.start(100, true);
    a.check("21", ctl.wait(200) == &op);
    t.waitAsync(ctl, op);
    a.check("22", ctl.wait(200) == &op);
    t.waitAsync(ctl, op);
    a.check("23", ctl.wait(200) == &op);
    t.waitAsync(ctl, op);

    // Stop it.
    t.stop();
    a.check("31", ctl.wait(100) == 0);

    // Make auto-repeat and let it fire often.
    t.start(100, true);
    afl::sys::Thread::sleep(500);
    a.check("41", ctl.wait(0) == &op);
    t.waitAsync(ctl, op);
    a.check("42", ctl.wait(0) == &op);
    t.waitAsync(ctl, op);
    a.check("43", ctl.wait(0) == &op);
}
