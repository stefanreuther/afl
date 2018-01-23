/**
  *  \file u/t_test_socket.cpp
  *  \brief Test for afl::test::Socket
  */

#include "afl/test/socket.hpp"

#include "t_test.hpp"
#include "afl/test/assert.hpp"
#include "afl/net/name.hpp"
#include "afl/string/string.hpp"
#include "afl/async/controller.hpp"
#include "afl/except/assertionfailedexception.hpp"

/** Test success cases. */
void
TestTestSocket::testSuccess()
{
    afl::base::Ref<afl::net::Socket> testee = afl::test::Socket::create("a", afl::net::Name("n", "p"), afl::string::toBytes("abc"), afl::string::toBytes("xyz"));

    // Self-information
    TS_ASSERT_EQUALS(testee->getPeerName().toString(), "n:p");
    TS_ASSERT_EQUALS(testee->getName(), "n:p");

    // Send
    afl::async::Controller ctl;
    TS_ASSERT_THROWS_NOTHING(testee->fullSend(ctl, afl::string::toBytes("xyz")));

    // Receive
    uint8_t rx[3];
    TS_ASSERT_THROWS_NOTHING(testee->fullReceive(ctl, rx));
    TS_ASSERT_SAME_DATA(rx, "abc", 3);

    // closeSend
    TS_ASSERT_THROWS_NOTHING(testee->closeSend());
}

/** Test failure cases. */
void
TestTestSocket::testFailure()
{
    const afl::net::Name name("n", "p");
    const afl::base::ConstBytes_t rx(afl::string::toBytes("abc"));
    const afl::base::ConstBytes_t tx(afl::string::toBytes("xyz"));

    // closeSend although still data scheduled for send
    TS_ASSERT_THROWS(afl::test::Socket::create("a", name, rx, tx)->closeSend(), afl::except::AssertionFailedException);

    // sending wrong data
    afl::async::Controller ctl;
    TS_ASSERT_THROWS(afl::test::Socket::create("a", name, rx, tx)->fullSend(ctl, afl::string::toBytes("zzz")), afl::except::AssertionFailedException);
}

