/**
  *  \file u/t_async_operation.cpp
  *  \brief Test for afl::async::Operation
  */

#include "afl/async/operation.hpp"

#include "u/t_async.hpp"
#include "afl/async/controller.hpp"

/** Simple test. */
void
TestAsyncOperation::testIt()
{
    /* It's an interface; we only test that the header file is valid
       and we can instantiate the class. */
    afl::async::Operation op;
    TS_ASSERT(op.getController() == 0);

    // Trivial setter/getter test
    afl::async::Controller ctl;
    op.setController(&ctl);
    TS_ASSERT(op.getController() == &ctl);
    op.setController(0);
    TS_ASSERT(op.getController() == 0);
}
