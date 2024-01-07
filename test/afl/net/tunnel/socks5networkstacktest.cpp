/**
  *  \file test/afl/net/tunnel/socks5networkstacktest.cpp
  *  \brief Test for afl::net::tunnel::Socks5NetworkStack
  */

#include "afl/net/tunnel/socks5networkstack.hpp"

#include "afl/async/controller.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/except/unsupportedexception.hpp"
#include "afl/net/nullnetworkstack.hpp"
#include "afl/test/networkstack.hpp"
#include "afl/test/testrunner.hpp"

namespace {
    void verifyFinish(afl::test::Assert a, afl::async::CommunicationObject& obj)
    {
        // Send 'a','b'
        afl::async::Controller ctl;
        static const uint8_t PAYLOAD[] = {'a','b'};
        AFL_CHECK_SUCCEEDS(a("fullSend"), obj.fullSend(ctl, PAYLOAD));

        // Receive 'x','y'
        uint8_t reply[2];
        AFL_CHECK_SUCCEEDS(a("fullReceive"), obj.fullReceive(ctl, reply));
        a.checkEqual("received 1", reply[0], 'x');
        a.checkEqual("received 2", reply[1], 'y');
    }
}

/** Test connect(), IP case. */
AFL_TEST("afl.net.tunnel.Socks5NetworkStack:connect:ip", a)
{
    afl::test::NetworkStack mock("testConnect");
    afl::net::tunnel::Socks5NetworkStack testee(mock, afl::net::Name("s5host", "s5port"));

    // Expect a connection that wants to connect to 141.30.6.1:8080
    static const uint8_t TX[] = {5,1,0, 5,1,0,1,141,30,6,1,0x1F,0x90,'a','b'};
    static const uint8_t RX[] = {5,0, 5,0,0,1,7,6,5,4,99,22,'x','y'};
    mock.expectConnection(afl::net::Name("s5host", "s5port"), RX, TX);

    // Connect
    afl::base::Ref<afl::net::Socket> sock(testee.connect(afl::net::Name::parse("141.30.6.1:8080", "foo"), 10));
    verifyFinish(a, *sock);
}

/** Test connect(), domain case.
    Also tests that the server replies to the operation with a domain address. */
AFL_TEST("afl.net.tunnel.Socks5NetworkStack:connect:domain", a)
{
    afl::test::NetworkStack mock("testConnectDomain");
    afl::net::tunnel::Socks5NetworkStack testee(mock, afl::net::Name("s5host", "s5port"));

    // Expect a connection that wants to connect to 141.30.6.1:8080
    static const uint8_t TX[] = {5,1,0, 5,1,0,3,'d','o','m','a','i','n',0,0x1F,0x90,'a','b'};
    static const uint8_t RX[] = {5,0, 5,0,0,3,'a','d','d','r','e','s','s',0,99,22,'x','y'};
    mock.expectConnection(afl::net::Name("s5host", "s5port"), RX, TX);

    // Connect
    afl::base::Ref<afl::net::Socket> sock(testee.connect(afl::net::Name::parse("domain:8080", "foo"), 10));
    verifyFinish(a, *sock);
}

/** Test connect(), invalid port case. */
AFL_TEST("afl.net.tunnel.Socks5NetworkStack:connect:invalid-port", a)
{
    afl::test::NetworkStack mock("testConnectInvalid");
    afl::net::tunnel::Socks5NetworkStack testee(mock, afl::net::Name("s5host", "s5port"));

    // A connection will still be made
    mock.expectConnection(afl::net::Name("s5host", "s5port"), afl::base::Nothing, afl::base::Nothing);

    // Connect
    AFL_CHECK_THROWS(a("connet"), testee.connect(afl::net::Name("141.30.6.1", "foo"), 10), afl::except::FileProblemException);
}

/** Test connect(), server rejects connection. */
AFL_TEST("afl.net.tunnel.Socks5NetworkStack:connect:server-rejects", a)
{
    afl::test::NetworkStack mock("testConnectReject");
    afl::net::tunnel::Socks5NetworkStack testee(mock, afl::net::Name("s5host", "s5port"));

    // Expect a connection that wants to connect to 141.30.6.1:8080
    static const uint8_t TX[] = {5,1,0, 5,1,0,1,141,30,6,1,0x1F,0x90,'a','b'};
    static const uint8_t RX[] = {5,0, 5,4,0,1,7,6,5,4,99,22,'x','y'};
    mock.expectConnection(afl::net::Name("s5host", "s5port"), RX, TX);

    // Connect
    AFL_CHECK_THROWS(a("connect"), testee.connect(afl::net::Name::parse("141.30.6.1:8080", "foo"), 10), afl::except::FileProblemException);
}

/** Test connect(), server drops connection. */
AFL_TEST("afl.net.tunnel.Socks5NetworkStack:connect:server-closes", a)
{
    afl::test::NetworkStack mock("testConnectClose");
    afl::net::tunnel::Socks5NetworkStack testee(mock, afl::net::Name("s5host", "s5port"));

    // Expect a connection that wants to connect to 141.30.6.1:8080
    static const uint8_t TX[] = {5,1,0, 5,1,0,1,141,30,6,1,0x1F,0x90,'a','b'};
    static const uint8_t RX[] = {5,0};
    mock.expectConnection(afl::net::Name("s5host", "s5port"), RX, TX);

    // Connect
    AFL_CHECK_THROWS(a("connect"), testee.connect(afl::net::Name::parse("141.30.6.1:8080", "foo"), 10), afl::except::FileProblemException);
}

/** Test connect(), no acceptable authentification. */
AFL_TEST("afl.net.tunnel.Socks5NetworkStack:connect:auth-mismatch", a)
{
    afl::test::NetworkStack mock("testConnectAuthMismatch");
    afl::net::tunnel::Socks5NetworkStack testee(mock, afl::net::Name("s5host", "s5port"));

    // Expect a connection that wants to connect to 141.30.6.1:8080
    static const uint8_t TX[] = {5,1,0};
    static const uint8_t RX[] = {5,0xFF};
    mock.expectConnection(afl::net::Name("s5host", "s5port"), RX, TX);

    // Connect
    AFL_CHECK_THROWS(a("connect"), testee.connect(afl::net::Name::parse("141.30.6.1:8080", "foo"), 10), afl::except::FileProblemException);
}

/** Test connect(), server drops connection in authentification. */
AFL_TEST("afl.net.tunnel.Socks5NetworkStack:connect:server-aborts-auth", a)
{
    afl::test::NetworkStack mock("testConnectAuthClose");
    afl::net::tunnel::Socks5NetworkStack testee(mock, afl::net::Name("s5host", "s5port"));

    // Expect a connection that wants to connect to 141.30.6.1:8080
    static const uint8_t TX[] = {5,1,0};
    mock.expectConnection(afl::net::Name("s5host", "s5port"), afl::base::Nothing, TX);

    // Connect
    AFL_CHECK_THROWS(a("connect"), testee.connect(afl::net::Name::parse("141.30.6.1:8080", "foo"), 10), afl::except::FileProblemException);
}

/** Test connect(), unreachable server. */
AFL_TEST("afl.net.tunnel.Socks5NetworkStack:connect:server-unreachable", a)
{
    afl::net::NullNetworkStack null;
    afl::net::tunnel::Socks5NetworkStack testee(null, afl::net::Name("s5host", "s5port"));
    AFL_CHECK_THROWS(a("connect"), testee.connect(afl::net::Name("a", "1"), 10), afl::except::FileProblemException);
}

/** Test listen(), rejected. */
AFL_TEST("afl.net.tunnel.Socks5NetworkStack:listen", a)
{
    afl::test::NetworkStack mock("testListen");
    afl::net::tunnel::Socks5NetworkStack testee(mock, afl::net::Name("s5host", "s5port"));
    AFL_CHECK_THROWS(a("listen"), testee.listen(afl::net::Name("0.0.0.0", 10000), 10), afl::except::UnsupportedException);
}
