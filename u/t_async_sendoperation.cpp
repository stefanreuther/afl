/**
  *  \file u/t_async_sendoperation.cpp
  *  \brief Test for afl::async::SendOperation
  */

#include "afl/async/sendoperation.hpp"

#include "u/t_async.hpp"

/** Simple test. */
void
TestAsyncSendOperation::testIt()
{
    // Default construction
    {
        afl::async::SendOperation op;
        TS_ASSERT_EQUALS(op.getNumSentBytes(), 0U);
        TS_ASSERT(op.isCompleted());
    }

    // With data
    {
        uint8_t d[] = { 1,2,3 };
        afl::async::SendOperation op(d);
        TS_ASSERT_EQUALS(op.getNumSentBytes(), 0U);
        TS_ASSERT(!op.isCompleted());
        TS_ASSERT_EQUALS(op.getUnsentBytes().size(), 3U);

        // Pretend we send something
        op.addSentBytes(2);
        TS_ASSERT(!op.isCompleted());
        TS_ASSERT_EQUALS(op.getUnsentBytes().size(), 1U);
        TS_ASSERT_EQUALS(op.getNumSentBytes(), 2U);

        // Check what we send; it doesn't modify the SendOperation
        TS_ASSERT_EQUALS(*op.getUnsentBytes().eat(), 3U);
        TS_ASSERT_EQUALS(op.getUnsentBytes().size(), 1U);

        // Finish
        op.addSentBytes(1);
        TS_ASSERT(op.isCompleted());
        TS_ASSERT_EQUALS(op.getNumSentBytes(), 3U);

        // Reset
        op.setData(d);
        TS_ASSERT_EQUALS(op.getNumSentBytes(), 0U);
        TS_ASSERT(!op.isCompleted());
        TS_ASSERT_EQUALS(op.getUnsentBytes().size(), 3U);
    }
}
