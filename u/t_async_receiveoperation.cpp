/**
  *  \file u/t_async_receiveoperation.cpp
  *  \brief Test for afl::afl::ReceiveOperation
  */

#include "afl/async/receiveoperation.hpp"

#include "u/t_async.hpp"
#include "afl/async/sendoperation.hpp"

/** Simple test. */
void
TestAsyncReceiveOperation::testIt()
{
    // Default construction
    {
        afl::async::ReceiveOperation op;
        TS_ASSERT_EQUALS(op.getNumReceivedBytes(), 0U);
        TS_ASSERT(op.isCompleted());
    }

    // With data
    {
        uint8_t d[3];
        afl::async::ReceiveOperation op(d);
        TS_ASSERT_EQUALS(op.getNumReceivedBytes(), 0U);
        TS_ASSERT(!op.isCompleted());
        TS_ASSERT_EQUALS(op.getUnreceivedBytes().size(), 3U);

        // Pretend we receive something
        op.addReceivedBytes(2);
        TS_ASSERT(!op.isCompleted());
        TS_ASSERT_EQUALS(op.getUnreceivedBytes().size(), 1U);
        TS_ASSERT_EQUALS(op.getNumReceivedBytes(), 2U);

        // Finish
        op.addReceivedBytes(1);
        TS_ASSERT(op.isCompleted());
        TS_ASSERT_EQUALS(op.getNumReceivedBytes(), 3U);

        // Reset
        op.setData(d);
        TS_ASSERT_EQUALS(op.getNumReceivedBytes(), 0U);
        TS_ASSERT(!op.isCompleted());
        TS_ASSERT_EQUALS(op.getUnreceivedBytes().size(), 3U);
    }
}

/** Test interaction with SendOperation. */
void
TestAsyncReceiveOperation::testCopy()
{
    // Matching copy
    {
        const uint8_t src[] = { 1, 2, 3 };
        uint8_t dst[3] = {};
        afl::async::SendOperation sender(src);
        afl::async::ReceiveOperation receiver(dst);
        TS_ASSERT(!sender.isCompleted());
        TS_ASSERT(!receiver.isCompleted());
        receiver.copyFrom(sender);

        TS_ASSERT(sender.isCompleted());
        TS_ASSERT(receiver.isCompleted());
        TS_ASSERT_EQUALS(sender.getNumSentBytes(), 3U);
        TS_ASSERT_EQUALS(receiver.getNumReceivedBytes(), 3U);
        TS_ASSERT_SAME_DATA(src, dst, 3);
    }

    // Different sizes and restarting in the middle
    {
        const uint8_t src[] = { 1, 2, 3, 4, 5 };
        uint8_t dst[3] = {};
        afl::async::SendOperation sender(src);
        afl::async::ReceiveOperation receiver(dst);
        TS_ASSERT(!sender.isCompleted());
        TS_ASSERT(!receiver.isCompleted());
        receiver.copyFrom(sender);

        TS_ASSERT(!sender.isCompleted());
        TS_ASSERT(receiver.isCompleted());
        TS_ASSERT_EQUALS(sender.getNumSentBytes(), 3U);
        TS_ASSERT_EQUALS(receiver.getNumReceivedBytes(), 3U);
        TS_ASSERT_SAME_DATA(src, dst, 3);

        // Try continueing
        receiver.setData(dst);
        receiver.copyFrom(sender);

        TS_ASSERT(sender.isCompleted());
        TS_ASSERT(!receiver.isCompleted());
        TS_ASSERT_EQUALS(sender.getNumSentBytes(), 5U);
        TS_ASSERT_EQUALS(receiver.getNumReceivedBytes(), 2U);
        TS_ASSERT_SAME_DATA(src+3, dst, 2);
    }
}
