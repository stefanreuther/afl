/**
  *  \file test/afl/test/sockettest.cpp
  *  \brief Test for afl::test::Socket
  */

#include "afl/test/socket.hpp"

#include "afl/async/controller.hpp"
#include "afl/except/assertionfailedexception.hpp"
#include "afl/net/name.hpp"
#include "afl/string/string.hpp"
#include "afl/test/testrunner.hpp"

/** Test success cases. */
AFL_TEST("afl.test.Socket:success", a) {
    afl::base::Ref<afl::net::Socket> testee = afl::test::Socket::create("a", afl::net::Name("n", "p"), afl::string::toBytes("abc"), afl::string::toBytes("xyz"));

    // Self-information
    a.checkEqual("getPeerName", testee->getPeerName().toString(), "n:p");
    a.checkEqual("getName",     testee->getName(), "n:p");

    // Send
    afl::async::Controller ctl;
    AFL_CHECK_SUCCEEDS(a("fullSend"), testee->fullSend(ctl, afl::string::toBytes("xyz")));

    // Receive
    uint8_t rx[3];
    AFL_CHECK_SUCCEEDS(a("fullReceive"), testee->fullReceive(ctl, rx));
    a.checkEqualContent("received-data", afl::base::ConstBytes_t(rx), afl::string::toBytes("abc"));

    // closeSend
    AFL_CHECK_SUCCEEDS(a("closeSend"), testee->closeSend());
}

/** Test failure cases. */
AFL_TEST("afl.test.Socket:fail", a) {

    const afl::net::Name name("n", "p");
    const afl::base::ConstBytes_t rx(afl::string::toBytes("abc"));
    const afl::base::ConstBytes_t tx(afl::string::toBytes("xyz"));

    // closeSend although still data scheduled for send
    AFL_CHECK_THROWS(a("early-close"), afl::test::Socket::create("a", name, rx, tx)->closeSend(), afl::except::AssertionFailedException);

    // sending wrong data
    afl::async::Controller ctl;
    AFL_CHECK_THROWS(a("wrong-data"), afl::test::Socket::create("a", name, rx, tx)->fullSend(ctl, afl::string::toBytes("zzz")), afl::except::AssertionFailedException);
}
