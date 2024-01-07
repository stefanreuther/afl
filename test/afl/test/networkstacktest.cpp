/**
  *  \file test/afl/test/networkstacktest.cpp
  *  \brief Test for afl::test::NetworkStack
  */

#include "afl/test/networkstack.hpp"
#include "afl/test/testrunner.hpp"
#include "afl/except/assertionfailedexception.hpp"
#include "afl/async/controller.hpp"

/** Test success case. */
AFL_TEST("afl.test.NetworkStack:success", a) {
    // Set up
    afl::test::NetworkStack testee("a");
    testee.expectConnection(afl::net::Name("n", "p"), afl::string::toBytes("abc"), afl::string::toBytes("xyz"));

    // Connect
    afl::base::Ref<afl::net::Socket> t = testee.connect(afl::net::Name("n", "p"), afl::sys::INFINITE_TIMEOUT);

    // Self-information
    a.checkEqual("getPeerName", t->getPeerName().toString(), "n:p");
    a.checkEqual("getName",     t->getName(), "n:p");

    // Send
    afl::async::Controller ctl;
    AFL_CHECK_SUCCEEDS(a("fullSend"), t->fullSend(ctl, afl::string::toBytes("xyz")));

    // Receive
    uint8_t rx[3];
    AFL_CHECK_SUCCEEDS(a("fullReceive"), t->fullReceive(ctl, rx));
    a.checkEqualContent("received-data", afl::base::ConstBytes_t(rx), afl::string::toBytes("abc"));
}

/** Test failure cases. */
AFL_TEST("afl.test.NetworkStack:fail", a) {
    afl::test::NetworkStack testee("a");

    // Unexpected connection request
    AFL_CHECK_THROWS(a("unexpected-connect"), testee.connect(afl::net::Name("x", "y"), afl::sys::INFINITE_TIMEOUT), afl::except::AssertionFailedException);

    // Listen is not supported
    AFL_CHECK_THROWS(a("listen"), testee.listen(afl::net::Name("x", "y"), 5), afl::except::AssertionFailedException);
}

/** Test failure case: connect to wrong target. */
AFL_TEST("afl.test.NetworkStack:fail-wrong-target", a) {
    afl::test::NetworkStack testee("a");
    testee.expectConnection(afl::net::Name("n", "p"), afl::string::toBytes("abc"), afl::string::toBytes("xyz"));
    AFL_CHECK_THROWS(a, testee.connect(afl::net::Name("u", "v"), afl::sys::INFINITE_TIMEOUT), afl::except::AssertionFailedException);
}

/** Test failure case: using wrong data. */
AFL_TEST("afl.test.NetworkStack:fail-wrong-data", a) {
    afl::test::NetworkStack testee("a");
    testee.expectConnection(afl::net::Name("n", "p"), afl::string::toBytes("abc"), afl::string::toBytes("xyz"));
    afl::base::Ref<afl::net::Socket> t = testee.connect(afl::net::Name("n", "p"), afl::sys::INFINITE_TIMEOUT);

    // Send wrong data
    afl::async::Controller ctl;
    AFL_CHECK_THROWS(a, t->fullSend(ctl, afl::string::toBytes("qqq")), afl::except::AssertionFailedException);
}
