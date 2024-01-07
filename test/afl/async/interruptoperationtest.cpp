/**
  *  \file test/afl/async/interruptoperationtest.cpp
  *  \brief Test for afl::async::InterruptOperation
  */

#include "afl/async/interruptoperation.hpp"
#include "afl/test/testrunner.hpp"

using afl::async::InterruptOperation;

// Default constructor
AFL_TEST("afl.async.InterruptOperation:default", a)
{
    InterruptOperation op;
    a.check("getRequested", op.getRequested() == InterruptOperation::Kinds_t());
    a.check("getReceived",  op.getReceived() == InterruptOperation::Kinds_t());
}

// Unit constructor
AFL_TEST("afl.async.InterruptOperation:unit", a)
{
    InterruptOperation op(InterruptOperation::Terminate);
    a.check("getRequested", op.getRequested() == InterruptOperation::Kinds_t(InterruptOperation::Terminate));
    a.check("getReceived" , op.getReceived() == InterruptOperation::Kinds_t());
}

// Set constructor
AFL_TEST("afl.async.InterruptOperation:set", a)
{
    InterruptOperation op(InterruptOperation::Kinds_t(InterruptOperation::Terminate) + InterruptOperation::Break);
    a.check("getRequested", op.getRequested() == InterruptOperation::Kinds_t(InterruptOperation::Terminate) + InterruptOperation::Break);
    a.check("getReceived",  op.getReceived() == InterruptOperation::Kinds_t());
}

// Modifiers
AFL_TEST("afl.async.InterruptOperation:modifiers", a)
{
    InterruptOperation op;
    op.setRequested(InterruptOperation::Kinds_t(InterruptOperation::Terminate));
    op.setReceived(InterruptOperation::Kinds_t(InterruptOperation::Break));
    a.check("getRequested", op.getRequested() == InterruptOperation::Kinds_t(InterruptOperation::Terminate));
    a.check("getReceived",  op.getReceived() == InterruptOperation::Kinds_t(InterruptOperation::Break));
}
