/**
  *  \file test/afl/test/commandhandlertest.cpp
  *  \brief Test for afl::test::CommandHandler
  */

#include "afl/test/commandhandler.hpp"
#include "afl/data/booleanvalue.hpp"
#include "afl/data/floatvalue.hpp"
#include "afl/data/hash.hpp"
#include "afl/data/hashvalue.hpp"
#include "afl/data/integervalue.hpp"
#include "afl/data/vector.hpp"
#include "afl/data/vectorvalue.hpp"
#include "afl/except/assertionfailedexception.hpp"
#include "afl/test/testrunner.hpp"

using afl::data::IntegerValue;
using afl::data::Segment;
using afl::except::AssertionFailedException;
using afl::test::CommandHandler;

/** Test success cases. */
AFL_TEST("afl.test.CommandHandler:success", a) {
    CommandHandler testee("z");

    // One-element
    testee.expectCall("A");
    testee.provideNewResult(new IntegerValue(7));
    a.checkEqual("0-args-ret-int", testee.callInt(Segment().pushBackString("A")), 7);

    // Two-element
    testee.expectCall("X, 9");
    testee.provideNewResult(new IntegerValue(12));
    a.checkEqual("1-arg-ret-int", testee.callInt(Segment().pushBackString("X").pushBackInteger(9)), 12);

    // Void return
    testee.expectCall("Q");
    testee.provideNewResult(new IntegerValue(42));
    AFL_CHECK_SUCCEEDS(a("ret-void"), testee.callVoid(Segment().pushBackString("Q")));

    // Finish
    AFL_CHECK_SUCCEEDS(a("checkFinish"), testee.checkFinish());
}

/** Test failure case: wrong command. */
AFL_TEST("afl.test.CommandHandler:fail-wrong-command", a) {
    CommandHandler testee("z");
    testee.expectCall("A");
    testee.provideNewResult(new IntegerValue(42));
    AFL_CHECK_THROWS(a, testee.callInt(Segment().pushBackString("B")), AssertionFailedException);
}

/** Test failure case: no command expected. */
AFL_TEST("afl.test.CommandHandler:command-unexpected", a) {
    CommandHandler testee("z");
    AFL_CHECK_THROWS(a, testee.callInt(Segment().pushBackString("B")), AssertionFailedException);
}

/** Test failure case: no result. */
AFL_TEST("afl.test.CommandHandler:no-result", a) {
    CommandHandler testee("z");
    testee.expectCall("A");
    AFL_CHECK_THROWS(a, testee.callInt(Segment().pushBackString("A")), AssertionFailedException);
}

/** Test failure case: outstanding command. */
AFL_TEST("afl.test.CommandHandler:outstanding-command", a) {
    CommandHandler testee("z");
    testee.expectCall("A");
    AFL_CHECK_THROWS(a, testee.checkFinish(), afl::except::AssertionFailedException);
}

/** Test failure case: outstanding result. */
AFL_TEST("afl.test.CommandHandler:outstanding-result", a) {
    CommandHandler testee("z");
    testee.provideNewResult(new IntegerValue(42));
    AFL_CHECK_THROWS(a, testee.checkFinish(), afl::except::AssertionFailedException);
}

/** Test parameter types. */
AFL_TEST("afl.test.CommandHandler:parameter-types", a) {
    CommandHandler testee("z");

    // Integer
    testee.expectCall("Q, 77");
    testee.provideNewResult(new IntegerValue(42));
    AFL_CHECK_SUCCEEDS(a("int"), testee.callVoid(Segment().pushBackString("Q").pushBackInteger(77)));

    // String
    testee.expectCall("Q, abc");
    testee.provideNewResult(new IntegerValue(42));
    AFL_CHECK_SUCCEEDS(a("string"), testee.callVoid(Segment().pushBackString("Q").pushBackString("abc")));

    // Float
    testee.expectCall("Q, 2.50");
    testee.provideNewResult(new IntegerValue(42));
    AFL_CHECK_SUCCEEDS(a("float"), testee.callVoid(Segment().pushBackString("Q").pushBackNew(new afl::data::FloatValue(2.5))));

    // Bool
    testee.expectCall("Q, 1");
    testee.provideNewResult(new IntegerValue(42));
    AFL_CHECK_SUCCEEDS(a("bool"), testee.callVoid(Segment().pushBackString("Q").pushBackNew(new afl::data::BooleanValue(true))));

    // Vector
    testee.expectCall("Q, <vector>");
    testee.provideNewResult(new IntegerValue(42));
    AFL_CHECK_SUCCEEDS(a("vector"), testee.callVoid(Segment().pushBackString("Q").pushBackNew(new afl::data::VectorValue(afl::data::Vector::create()))));

    // Hash
    testee.expectCall("Q, <hash>");
    testee.provideNewResult(new IntegerValue(42));
    AFL_CHECK_SUCCEEDS(a("hash"), testee.callVoid(Segment().pushBackString("Q").pushBackNew(new afl::data::HashValue(afl::data::Hash::create()))));

    // Null
    testee.expectCall("Q, <null>");
    testee.provideNewResult(new IntegerValue(42));
    AFL_CHECK_SUCCEEDS(a("null"), testee.callVoid(Segment().pushBackString("Q").pushBackNew(0)));
}

