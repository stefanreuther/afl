/**
  *  \file test/afl/async/interrupttest.cpp
  *  \brief Test for afl::async::Interrupt
  */

#include "afl/async/interrupt.hpp"
#include "afl/test/testrunner.hpp"

#include "afl/async/controller.hpp"

/** Interface test. */
AFL_TEST("afl.async.Interrupt", a)
{
    // This waits for the "hangup" signal for 50 ms.
    // Normally, no such signal will be generated, but this exercises the required code paths.
    afl::async::Interrupt& i = afl::async::Interrupt::getInstance();
    afl::async::Controller ctl;

    afl::async::InterruptOperation::Kinds_t received =
        i.wait(ctl, afl::async::InterruptOperation::Kinds_t(afl::async::InterruptOperation::Hangup), 50);
    a.check("", received.empty());
}
