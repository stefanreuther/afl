/**
  *  \file u/t_async_interruptoperation.cpp
  *  \brief Test for afl::async::InterruptOperation
  */

#include "afl/async/interruptoperation.hpp"

#include "t_async.hpp"

/** Simple test.
    This is just a data class. */
void
TestAsyncInterruptOperation::testIt()
{
    using afl::async::InterruptOperation;

    // Default constructor
    {
        InterruptOperation op;
        TS_ASSERT_EQUALS(op.getRequested(), InterruptOperation::Kinds_t());
        TS_ASSERT_EQUALS(op.getReceived(), InterruptOperation::Kinds_t());
    }

    // Unit constructor
    {
        InterruptOperation op(InterruptOperation::Terminate);
        TS_ASSERT_EQUALS(op.getRequested(), InterruptOperation::Kinds_t(InterruptOperation::Terminate));
        TS_ASSERT_EQUALS(op.getReceived(), InterruptOperation::Kinds_t());
    }

    // Set constructor
    {
        InterruptOperation op(InterruptOperation::Kinds_t(InterruptOperation::Terminate) + InterruptOperation::Break);
        TS_ASSERT_EQUALS(op.getRequested(), InterruptOperation::Kinds_t(InterruptOperation::Terminate) + InterruptOperation::Break);
        TS_ASSERT_EQUALS(op.getReceived(), InterruptOperation::Kinds_t());
    }

    // Modifiers
    {
        InterruptOperation op;
        op.setRequested(InterruptOperation::Kinds_t(InterruptOperation::Terminate));
        op.setReceived(InterruptOperation::Kinds_t(InterruptOperation::Break));
        TS_ASSERT_EQUALS(op.getRequested(), InterruptOperation::Kinds_t(InterruptOperation::Terminate));
        TS_ASSERT_EQUALS(op.getReceived(), InterruptOperation::Kinds_t(InterruptOperation::Break));
    }
}

