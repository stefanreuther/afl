/**
  *  \file u/t_test_callreceiver.cpp
  *  \brief Test for afl::test::CallReceiver
  */

#include "afl/test/callreceiver.hpp"

#include "t_test.hpp"
#include "afl/except/assertionfailedexception.hpp"

/** Test success case. */
void
TestTestCallReceiver::testSuccess()
{
    afl::test::CallReceiver testee("ho");

    // Configure
    TS_ASSERT_THROWS_NOTHING(testee.expectCall("x"));
    TS_ASSERT_THROWS_NOTHING(testee.provideReturnValue(3));

    // Consume
    TS_ASSERT_THROWS_NOTHING(testee.checkCall("x"));
    TS_ASSERT_EQUALS(testee.consumeReturnValue<int>(), 3);

    // Finish
    TS_ASSERT_THROWS_NOTHING(testee.checkFinish());
}

/** Test failure case: unexpected call. */
void
TestTestCallReceiver::testFailUnexpected()
{
    afl::test::CallReceiver testee("ho");
    TS_ASSERT_THROWS(testee.checkCall("z"), afl::except::AssertionFailedException);
}

/** Test failure case: different call than expected. */
void
TestTestCallReceiver::testFailDifferent()
{
    afl::test::CallReceiver testee("ho");
    testee.expectCall("a");
    TS_ASSERT_THROWS(testee.checkCall("b"), afl::except::AssertionFailedException);
}

/** Test failure case: no return value. */
void
TestTestCallReceiver::testFailNoReturn()
{
    afl::test::CallReceiver testee("ho");
    TS_ASSERT_THROWS(testee.consumeReturnValue<int>(), afl::except::AssertionFailedException);
}

/** Test failure case: wrong return value type. */
void
TestTestCallReceiver::testFailWrongRetun()
{
    afl::test::CallReceiver testee("ho");
    testee.provideReturnValue(9);
    TS_ASSERT_THROWS(testee.consumeReturnValue<float>(), afl::except::AssertionFailedException);
}

/** Test failure case: outstanding calls. */
void
TestTestCallReceiver::testFailOutstanding()
{
    afl::test::CallReceiver testee("ho");
    testee.expectCall("a");
    TS_ASSERT_THROWS(testee.checkFinish(), afl::except::AssertionFailedException);
}

/** Test failure case: outstanding returns. */
void
TestTestCallReceiver::testFailOutstandingReturn()
{
    afl::test::CallReceiver testee("ho");
    testee.provideReturnValue("a");
    TS_ASSERT_THROWS(testee.checkFinish(), afl::except::AssertionFailedException);
}

