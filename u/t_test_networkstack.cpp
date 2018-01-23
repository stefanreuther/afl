/**
  *  \file u/t_test_networkstack.cpp
  *  \brief Test for afl::test::NetworkStack
  */

#include "afl/test/networkstack.hpp"

#include "t_test.hpp"
#include "afl/except/assertionfailedexception.hpp"
#include "afl/async/controller.hpp"

/** Test success case. */
void
TestTestNetworkStack::testSuccess()
{
    // Set up
    afl::test::NetworkStack testee("a");
    testee.expectConnection(afl::net::Name("n", "p"), afl::string::toBytes("abc"), afl::string::toBytes("xyz"));

    // Connect
    afl::base::Ref<afl::net::Socket> t = testee.connect(afl::net::Name("n", "p"), afl::sys::INFINITE_TIMEOUT);

    // Self-information
    TS_ASSERT_EQUALS(t->getPeerName().toString(), "n:p");
    TS_ASSERT_EQUALS(t->getName(), "n:p");

    // Send
    afl::async::Controller ctl;
    TS_ASSERT_THROWS_NOTHING(t->fullSend(ctl, afl::string::toBytes("xyz")));

    // Receive
    uint8_t rx[3];
    TS_ASSERT_THROWS_NOTHING(t->fullReceive(ctl, rx));
    TS_ASSERT_SAME_DATA(rx, "abc", 3);
}

/** Test failure cases. */
void
TestTestNetworkStack::testFailure()
{
    afl::test::NetworkStack testee("a");

    // Unexpected connection request
    TS_ASSERT_THROWS(testee.connect(afl::net::Name("x", "y"), afl::sys::INFINITE_TIMEOUT), afl::except::AssertionFailedException);

    // Listen is not supported
    TS_ASSERT_THROWS(testee.listen(afl::net::Name("x", "y"), 5), afl::except::AssertionFailedException);
}

/** Test failure case: connect to wrong target. */
void
TestTestNetworkStack::testFailureWrongTarget()
{
    afl::test::NetworkStack testee("a");
    testee.expectConnection(afl::net::Name("n", "p"), afl::string::toBytes("abc"), afl::string::toBytes("xyz"));
    TS_ASSERT_THROWS(testee.connect(afl::net::Name("u", "v"), afl::sys::INFINITE_TIMEOUT), afl::except::AssertionFailedException);
}

/** Test failure case: using wrong data. */
void
TestTestNetworkStack::testFailureWrongData()
{
    afl::test::NetworkStack testee("a");
    testee.expectConnection(afl::net::Name("n", "p"), afl::string::toBytes("abc"), afl::string::toBytes("xyz"));
    afl::base::Ref<afl::net::Socket> t = testee.connect(afl::net::Name("n", "p"), afl::sys::INFINITE_TIMEOUT);

    // Send wrong data
    afl::async::Controller ctl;
    TS_ASSERT_THROWS(t->fullSend(ctl, afl::string::toBytes("qqq")), afl::except::AssertionFailedException);
}

