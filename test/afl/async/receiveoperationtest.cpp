/**
  *  \file test/afl/async/receiveoperationtest.cpp
  *  \brief Test for afl::async::ReceiveOperation
  */

#include "afl/async/receiveoperation.hpp"

#include "afl/async/sendoperation.hpp"
#include "afl/test/testrunner.hpp"

using afl::base::ConstBytes_t;

// Default construction
AFL_TEST("afl.async.ReceiveOperation:default", a)
{
    afl::async::ReceiveOperation op;
    a.checkEqual("getNumReceivedBytes", op.getNumReceivedBytes(), 0U);
    a.check("isCompleted", op.isCompleted());
}

// With data
AFL_TEST("afl.async.ReceiveOperation:data", a)
{
    uint8_t d[3];
    afl::async::ReceiveOperation op(d);
    a.checkEqual("01. getNumReceivedBytes", op.getNumReceivedBytes(), 0U);
    a.check("02. isCompleted", !op.isCompleted());
    a.checkEqual("03. getUnreceivedBytes", op.getUnreceivedBytes().size(), 3U);

    // Pretend we receive something
    op.addReceivedBytes(2);
    a.check("11. isCompleted", !op.isCompleted());
    a.checkEqual("12. getUnreceivedBytes", op.getUnreceivedBytes().size(), 1U);
    a.checkEqual("13. getNumReceivedBytes", op.getNumReceivedBytes(), 2U);

    // Finish
    op.addReceivedBytes(1);
    a.check("21. isCompleted", op.isCompleted());
    a.checkEqual("22. getNumReceivedBytes", op.getNumReceivedBytes(), 3U);

    // Reset
    op.setData(d);
    a.checkEqual("31. getNumReceivedBytes", op.getNumReceivedBytes(), 0U);
    a.check("32. isCompleted", !op.isCompleted());
    a.checkEqual("33. getUnreceivedBytes", op.getUnreceivedBytes().size(), 3U);
}

/** Test interaction with SendOperation. */

// Matching copy
AFL_TEST("afl.async.ReceiveOperation:send:match", a)
{
    const uint8_t src[] = { 1, 2, 3 };
    uint8_t dst[3] = {};
    afl::async::SendOperation sender(src);
    afl::async::ReceiveOperation receiver(dst);
    a.check("01. isCompleted", !sender.isCompleted());
    a.check("02. isCompleted", !receiver.isCompleted());
    receiver.copyFrom(sender);

    a.check("11. isCompleted", sender.isCompleted());
    a.check("12. isCompleted", receiver.isCompleted());
    a.checkEqual("13. getNumSentBytes", sender.getNumSentBytes(), 3U);
    a.checkEqual("14. getNumReceivedBytes", receiver.getNumReceivedBytes(), 3U);
    a.checkEqualContent("15. data", ConstBytes_t(src), ConstBytes_t(dst));
}

// Different sizes and restarting in the middle
AFL_TEST("afl.async.ReceiveOperation:send:different", a)
{
    const uint8_t src[] = { 1, 2, 3, 4, 5 };
    uint8_t dst[3] = {};
    afl::async::SendOperation sender(src);
    afl::async::ReceiveOperation receiver(dst);
    a.check("01. isCompleted", !sender.isCompleted());
    a.check("02. isCompleted", !receiver.isCompleted());
    receiver.copyFrom(sender);

    a.check("11. isCompleted", !sender.isCompleted());
    a.check("12. isCompleted", receiver.isCompleted());
    a.checkEqual("13. getNumSentBytes", sender.getNumSentBytes(), 3U);
    a.checkEqual("14. getNumReceivedBytes", receiver.getNumReceivedBytes(), 3U);
    a.checkEqualContent("15. data", ConstBytes_t(src).trim(3), ConstBytes_t(dst).trim(3));

    // Try continueing
    receiver.setData(dst);
    receiver.copyFrom(sender);

    a.check("21. isCompleted", sender.isCompleted());
    a.check("22. isCompleted", !receiver.isCompleted());
    a.checkEqual("23. getNumSentBytes", sender.getNumSentBytes(), 5U);
    a.checkEqual("24. getNumReceivedBytes", receiver.getNumReceivedBytes(), 2U);
    a.checkEqualContent("25. data", ConstBytes_t::unsafeCreate(src+3, 2), ConstBytes_t::unsafeCreate(dst, 2));
}
