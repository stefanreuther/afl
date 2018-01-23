/**
  *  \file u/t_test_commandhandler.cpp
  *  \brief Test for afl::test::CommandHandler
  */

#include "afl/test/commandhandler.hpp"

#include "t_test.hpp"
#include "afl/data/integervalue.hpp"
#include "afl/except/assertionfailedexception.hpp"

/** Test success cases. */
void
TestTestCommandHandler::testSuccess()
{
    afl::test::CommandHandler testee("z");

    // One-element
    testee.expectCall("A");
    testee.provideNewResult(new afl::data::IntegerValue(7));
    TS_ASSERT_EQUALS(testee.callInt(afl::data::Segment().pushBackString("A")), 7);

    // Two-element
    testee.expectCall("X, 9");
    testee.provideNewResult(new afl::data::IntegerValue(12));
    TS_ASSERT_EQUALS(testee.callInt(afl::data::Segment().pushBackString("X").pushBackInteger(9)), 12);

    // Void return
    testee.expectCall("Q");
    testee.provideNewResult(new afl::data::IntegerValue(42));
    TS_ASSERT_THROWS_NOTHING(testee.callVoid(afl::data::Segment().pushBackString("Q")));

    // Finish
    TS_ASSERT_THROWS_NOTHING(testee.checkFinish());
}

/** Test failure case: wrong command. */
void
TestTestCommandHandler::testFailWrongCommand()
{
    afl::test::CommandHandler testee("z");
    testee.expectCall("A");
    testee.provideNewResult(new afl::data::IntegerValue(42));
    TS_ASSERT_THROWS(testee.callInt(afl::data::Segment().pushBackString("B")), afl::except::AssertionFailedException);
}

/** Test failure case: no command expected. */
void
TestTestCommandHandler::testFailNoCommand()
{
    afl::test::CommandHandler testee("z");
    TS_ASSERT_THROWS(testee.callInt(afl::data::Segment().pushBackString("B")), afl::except::AssertionFailedException);
}

/** Test failure case: no result. */
void
TestTestCommandHandler::testFailNoResult()
{
    afl::test::CommandHandler testee("z");
    testee.expectCall("A");
    TS_ASSERT_THROWS(testee.callInt(afl::data::Segment().pushBackString("A")), afl::except::AssertionFailedException);
}

/** Test failure case: outstanding command. */
void
TestTestCommandHandler::testFailOutstandingCommand()
{
    afl::test::CommandHandler testee("z");
    testee.expectCall("A");
    TS_ASSERT_THROWS(testee.checkFinish(), afl::except::AssertionFailedException);
}

/** Test failure case: outstanding result. */
void
TestTestCommandHandler::testFailOutstandingResult()
{
    afl::test::CommandHandler testee("z");
    testee.provideNewResult(new afl::data::IntegerValue(42));
    TS_ASSERT_THROWS(testee.checkFinish(), afl::except::AssertionFailedException);
}

