/**
  *  \file test/afl/async/operationtest.cpp
  *  \brief Test for afl::async::Operation
  */

#include "afl/async/operation.hpp"

#include "afl/async/controller.hpp"
#include "afl/test/testrunner.hpp"

/** Simple test. */
AFL_TEST("afl.async.Operation", a)
{
    /* It's an interface; we only test that the header file is valid
       and we can instantiate the class. */
    afl::async::Operation op;
    a.check("01", op.getController() == 0);

    // Trivial setter/getter test
    afl::async::Controller ctl;
    op.setController(&ctl);
    a.check("02", op.getController() == &ctl);
    op.setController(0);
    a.check("03", op.getController() == 0);
}
