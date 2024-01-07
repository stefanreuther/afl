/**
  *  \file test/afl/async/sendoperationtest.cpp
  *  \brief Test for afl::async::SendOperation
  */

#include "afl/async/sendoperation.hpp"
#include "afl/test/testrunner.hpp"

// Default construction
AFL_TEST("afl.async.SendOperation:default", a)
{
    afl::async::SendOperation op;
    a.checkEqual("getNumSentBytes", op.getNumSentBytes(), 0U);
    a.check("isCompleted", op.isCompleted());
}

// With data
AFL_TEST("afl.async.SendOperation:data", a)
{
    uint8_t d[] = { 1,2,3 };
    afl::async::SendOperation op(d);
    a.checkEqual("01. getNumSentBytes", op.getNumSentBytes(), 0U);
    a.check("02. isCompleted", !op.isCompleted());
    a.checkEqual("03. getUnsentBytes", op.getUnsentBytes().size(), 3U);

    // Pretend we send something
    op.addSentBytes(2);
    a.check("11. isCompleted", !op.isCompleted());
    a.checkEqual("12. getUnsentBytes", op.getUnsentBytes().size(), 1U);
    a.checkEqual("13. getNumSentBytes", op.getNumSentBytes(), 2U);

    // Check what we send; it doesn't modify the SendOperation
    a.checkEqual("14. eat", *op.getUnsentBytes().eat(), 3U);
    a.checkEqual("15. eat", op.getUnsentBytes().size(), 1U);

    // Finish
    op.addSentBytes(1);
    a.check("21. isCompleted", op.isCompleted());
    a.checkEqual("22. getNumSentBytes", op.getNumSentBytes(), 3U);

    // Reset
    op.setData(d);
    a.checkEqual("31. getNumSentBytes", op.getNumSentBytes(), 0U);
    a.check("32. isCompleted", !op.isCompleted());
    a.checkEqual("33. getUnsentBytes", op.getUnsentBytes().size(), 3U);
}
