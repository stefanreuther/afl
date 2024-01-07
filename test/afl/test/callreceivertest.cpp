/**
  *  \file test/afl/test/callreceivertest.cpp
  *  \brief test for afl::test::CallReceiver
  */

#include "afl/test/callreceiver.hpp"

#include "afl/except/assertionfailedexception.hpp"
#include "afl/test/testrunner.hpp"

using afl::except::AssertionFailedException;
using afl::test::CallReceiver;

/** Test success case. */
AFL_TEST("afl.test.CallReceiver:success", a) {
    CallReceiver testee("ho");

    // Configure
    AFL_CHECK_SUCCEEDS(a("expectCall"),         testee.expectCall("x"));
    AFL_CHECK_SUCCEEDS(a("provideReturnValue"), testee.provideReturnValue(3));

    // Consume
    AFL_CHECK_SUCCEEDS(a("checkCall"), testee.checkCall("x"));
    a.checkEqual("consumeReturnValue", testee.consumeReturnValue<int>(), 3);

    // Finish
    AFL_CHECK_SUCCEEDS(a("checkFinish"), testee.checkFinish());
}

/** Test failure case: unexpected call. */
AFL_TEST("afl.test.CallReceiver:fail:unexpected-call", a) {
    CallReceiver testee("ho");
    AFL_CHECK_THROWS(a, testee.checkCall("z"), AssertionFailedException);
}

/** Test failure case: different call than expected. */
AFL_TEST("afl.test.CallReceiver:fail:different-call", a) {
    CallReceiver testee("ho");
    testee.expectCall("a");
    AFL_CHECK_THROWS(a, testee.checkCall("b"), AssertionFailedException);
}

/** Test failure case: no return value. */
AFL_TEST("afl.test.CallReceiver:fail:missing-return-value", a) {
    CallReceiver testee("ho");
    AFL_CHECK_THROWS(a, testee.consumeReturnValue<int>(), AssertionFailedException);
}

/** Test failure case: wrong return value type. */
AFL_TEST("afl.test.CallReceiver:fail:wrong-return-value", a) {
    CallReceiver testee("ho");
    testee.provideReturnValue(9);
    AFL_CHECK_THROWS(a, testee.consumeReturnValue<float>(), AssertionFailedException);
}

/** Test failure case: outstanding calls. */
AFL_TEST("afl.test.CallReceiver:fail:outstanding-call", a) {
    CallReceiver testee("ho");
    testee.expectCall("a");
    AFL_CHECK_THROWS(a, testee.checkFinish(), AssertionFailedException);
}

/** Test failure case: outstanding returns. */
AFL_TEST("afl.test.CallReceiver:fail:outstanding-return", a) {
    CallReceiver testee("ho");
    testee.provideReturnValue("a");
    AFL_CHECK_THROWS(a, testee.checkFinish(), AssertionFailedException);
}
