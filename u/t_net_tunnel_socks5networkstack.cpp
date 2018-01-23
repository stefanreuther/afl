/**
  *  \file u/t_net_tunnel_socks5networkstack.cpp
  *  \brief Test for afl::net::tunnel::Socks5NetworkStack
  */

#include "afl/net/tunnel/socks5networkstack.hpp"

#include "t_net_tunnel.hpp"
#include "afl/async/controller.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/except/unsupportedexception.hpp"
#include "afl/net/nullnetworkstack.hpp"
#include "afl/test/networkstack.hpp"

namespace {
    void verifyFinish(afl::async::CommunicationObject& obj)
    {
        // Send 'a','b'
        afl::async::Controller ctl;
        static const uint8_t PAYLOAD[] = {'a','b'};
        TS_ASSERT_THROWS_NOTHING(obj.fullSend(ctl, PAYLOAD));

        // Receive 'x','y'
        uint8_t reply[2];
        TS_ASSERT_THROWS_NOTHING(obj.fullReceive(ctl, reply));
        TS_ASSERT_EQUALS(reply[0], 'x');
        TS_ASSERT_EQUALS(reply[1], 'y');
    }
}

/** Test connect(), IP case. */
void
TestNetTunnelSocks5NetworkStack::testConnect()
{
    afl::test::NetworkStack mock("testConnect");
    afl::net::tunnel::Socks5NetworkStack testee(mock, afl::net::Name("s5host", "s5port"));

    // Expect a connection that wants to connect to 141.30.6.1:8080
    static const uint8_t TX[] = {5,1,0, 5,1,0,1,141,30,6,1,0x1F,0x90,'a','b'};
    static const uint8_t RX[] = {5,0, 5,0,0,1,7,6,5,4,99,22,'x','y'};
    mock.expectConnection(afl::net::Name("s5host", "s5port"), RX, TX);

    // Connect
    afl::base::Ref<afl::net::Socket> sock(testee.connect(afl::net::Name::parse("141.30.6.1:8080", "foo"), 10));
    verifyFinish(*sock);
}

/** Test connect(), domain case.
    Also tests that the server replies to the operation with a domain address. */
void
TestNetTunnelSocks5NetworkStack::testConnectDomain()
{
    afl::test::NetworkStack mock("testConnectDomain");
    afl::net::tunnel::Socks5NetworkStack testee(mock, afl::net::Name("s5host", "s5port"));

    // Expect a connection that wants to connect to 141.30.6.1:8080
    static const uint8_t TX[] = {5,1,0, 5,1,0,3,'d','o','m','a','i','n',0,0x1F,0x90,'a','b'};
    static const uint8_t RX[] = {5,0, 5,0,0,3,'a','d','d','r','e','s','s',0,99,22,'x','y'};
    mock.expectConnection(afl::net::Name("s5host", "s5port"), RX, TX);

    // Connect
    afl::base::Ref<afl::net::Socket> sock(testee.connect(afl::net::Name::parse("domain:8080", "foo"), 10));
    verifyFinish(*sock);
}

/** Test connect(), invalid port case. */
void
TestNetTunnelSocks5NetworkStack::testConnectInvalid()
{
    afl::test::NetworkStack mock("testConnectInvalid");
    afl::net::tunnel::Socks5NetworkStack testee(mock, afl::net::Name("s5host", "s5port"));

    // A connection will still be made
    mock.expectConnection(afl::net::Name("s5host", "s5port"), afl::base::Nothing, afl::base::Nothing);

    // Connect
    TS_ASSERT_THROWS(testee.connect(afl::net::Name("141.30.6.1", "foo"), 10), afl::except::FileProblemException);
}

/** Test connect(), server rejects connection. */
void
TestNetTunnelSocks5NetworkStack::testConnectReject()
{
    afl::test::NetworkStack mock("testConnectReject");
    afl::net::tunnel::Socks5NetworkStack testee(mock, afl::net::Name("s5host", "s5port"));

    // Expect a connection that wants to connect to 141.30.6.1:8080
    static const uint8_t TX[] = {5,1,0, 5,1,0,1,141,30,6,1,0x1F,0x90,'a','b'};
    static const uint8_t RX[] = {5,0, 5,4,0,1,7,6,5,4,99,22,'x','y'};
    mock.expectConnection(afl::net::Name("s5host", "s5port"), RX, TX);

    // Connect
    TS_ASSERT_THROWS(testee.connect(afl::net::Name::parse("141.30.6.1:8080", "foo"), 10), afl::except::FileProblemException);
}

/** Test connect(), server drops connection. */
void
TestNetTunnelSocks5NetworkStack::testConnectClose()
{
    afl::test::NetworkStack mock("testConnectClose");
    afl::net::tunnel::Socks5NetworkStack testee(mock, afl::net::Name("s5host", "s5port"));

    // Expect a connection that wants to connect to 141.30.6.1:8080
    static const uint8_t TX[] = {5,1,0, 5,1,0,1,141,30,6,1,0x1F,0x90,'a','b'};
    static const uint8_t RX[] = {5,0};
    mock.expectConnection(afl::net::Name("s5host", "s5port"), RX, TX);

    // Connect
    TS_ASSERT_THROWS(testee.connect(afl::net::Name::parse("141.30.6.1:8080", "foo"), 10), afl::except::FileProblemException);
}

/** Test connect(), no acceptable authentification. */
void
TestNetTunnelSocks5NetworkStack::testConnectAuthMismatch()
{
    afl::test::NetworkStack mock("testConnectAuthMismatch");
    afl::net::tunnel::Socks5NetworkStack testee(mock, afl::net::Name("s5host", "s5port"));

    // Expect a connection that wants to connect to 141.30.6.1:8080
    static const uint8_t TX[] = {5,1,0};
    static const uint8_t RX[] = {5,0xFF};
    mock.expectConnection(afl::net::Name("s5host", "s5port"), RX, TX);

    // Connect
    TS_ASSERT_THROWS(testee.connect(afl::net::Name::parse("141.30.6.1:8080", "foo"), 10), afl::except::FileProblemException);
}

/** Test connect(), server drops connection in authentification. */
void
TestNetTunnelSocks5NetworkStack::testConnectAuthClose()
{
    afl::test::NetworkStack mock("testConnectAuthClose");
    afl::net::tunnel::Socks5NetworkStack testee(mock, afl::net::Name("s5host", "s5port"));

    // Expect a connection that wants to connect to 141.30.6.1:8080
    static const uint8_t TX[] = {5,1,0};
    mock.expectConnection(afl::net::Name("s5host", "s5port"), afl::base::Nothing, TX);

    // Connect
    TS_ASSERT_THROWS(testee.connect(afl::net::Name::parse("141.30.6.1:8080", "foo"), 10), afl::except::FileProblemException);
}

/** Test connect(), unreachable server. */
void
TestNetTunnelSocks5NetworkStack::testConnectUnreachable()
{
    afl::net::NullNetworkStack null;
    afl::net::tunnel::Socks5NetworkStack testee(null, afl::net::Name("s5host", "s5port"));
    TS_ASSERT_THROWS(testee.connect(afl::net::Name("a", "1"), 10), afl::except::FileProblemException);
}

/** Test listen(), rejected. */
void
TestNetTunnelSocks5NetworkStack::testListen()
{
    afl::test::NetworkStack mock("testListen");
    afl::net::tunnel::Socks5NetworkStack testee(mock, afl::net::Name("s5host", "s5port"));
    TS_ASSERT_THROWS(testee.listen(afl::net::Name("0.0.0.0", 10000), 10), afl::except::UnsupportedException);
}
