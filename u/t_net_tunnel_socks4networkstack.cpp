/**
  *  \file u/t_net_tunnel_socks4networkstack.cpp
  *  \brief Test for afl::net::tunnel::Socks4NetworkStack
  */

#include <list>
#include <algorithm>
#include "afl/net/tunnel/socks4networkstack.hpp"

#include "t_net_tunnel.hpp"
#include "afl/async/controller.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/net/acceptoperation.hpp"
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
TestNetTunnelSocks4NetworkStack::testConnect()
{
    afl::test::NetworkStack mock("testConnect");
    afl::net::tunnel::Socks4NetworkStack testee(mock, afl::net::Name("s4host", "s4port"));

    // Expect a connection that wants to connect to 141.30.6.1:8080
    static const uint8_t TX[] = {4,1,0x1F,0x90,141,30,6,1,0,'a','b'};
    static const uint8_t RX[] = {0,90,77,77,77,77,77,77,'x','y'};
    mock.expectConnection(afl::net::Name("s4host", "s4port"), RX, TX);

    // Connect
    afl::base::Ref<afl::net::Socket> sock(testee.connect(afl::net::Name::parse("141.30.6.1:8080", "foo"), 10));
    verifyFinish(*sock);
}

/** Test connect(), domain name case.
    This must use the SOCKS4a format with an invalid IP address. */
void
TestNetTunnelSocks4NetworkStack::testConnectDomain()
{
    afl::test::NetworkStack mock("testConnectDomain");
    afl::net::tunnel::Socks4NetworkStack testee(mock, afl::net::Name("s4host", "s4port"));

    // Expect a connection that wants to connect to domain:77
    static const uint8_t TX[] = {4,1,0,77,0,0,0,1,0,'d','o','m','a','i','n',0,'a','b'};
    static const uint8_t RX[] = {0,90,77,77,77,77,77,77,'x','y'};
    mock.expectConnection(afl::net::Name("s4host", "s4port"), RX, TX);

    // Connect
    afl::base::Ref<afl::net::Socket> sock(testee.connect(afl::net::Name::parse("domain", "77"), 10));
    verifyFinish(*sock);
}

/** Test connect(), bad IP case.
    If the SOCKS4 format were used, the IP would have been mistaken as SOCKS4a format, so this must use SOCKS4a. */
void
TestNetTunnelSocks4NetworkStack::testConnectBadIP()
{
    afl::test::NetworkStack mock("testConnectBadIP");
    afl::net::tunnel::Socks4NetworkStack testee(mock, afl::net::Name("s4host", "s4port"));

    // Expect a connection that wants to connect to 0.0.0.42:23
    static const uint8_t TX[] = {4,1,0,23,0,0,0,1,0,'0','.','0','.','0','.','4','2',0,'a','b'};
    static const uint8_t RX[] = {0,90,77,77,77,77,77,77,'x','y'};
    mock.expectConnection(afl::net::Name("s4host", "s4port"), RX, TX);

    // Connect
    afl::base::Ref<afl::net::Socket> sock(testee.connect(afl::net::Name::parse("0.0.0.42:23", "77"), 10));
    verifyFinish(*sock);
}

/** Test connect(), invalid case. */
void
TestNetTunnelSocks4NetworkStack::testConnectInvalid()
{
    afl::test::NetworkStack mock("testConnectInvalid");
    afl::net::tunnel::Socks4NetworkStack testee(mock, afl::net::Name("s4host", "s4port"));

    // FIXME: This will make a connection, but not transfer any data
    mock.expectConnection(afl::net::Name("s4host", "s4port"), afl::base::Nothing, afl::base::Nothing);

    TS_ASSERT_THROWS(testee.connect(afl::net::Name("1.2.3.4", "huhu"), 10), afl::except::FileProblemException);
}

/** Test connect(), rejection by server. */
void
TestNetTunnelSocks4NetworkStack::testConnectReject()
{
    afl::test::NetworkStack mock("testConnectReject");
    afl::net::tunnel::Socks4NetworkStack testee(mock, afl::net::Name("s4host", "s4port"));

    // Expect a connection that wants to connect to 141.30.6.1:8080.
    // Server rejects.
    static const uint8_t TX[] = {4,1,0x1F,0x90,141,30,6,1,0};
    static const uint8_t RX[] = {0,91,77,77,77,77,77,77};
    mock.expectConnection(afl::net::Name("s4host", "s4port"), RX, TX);

    // Connect
    TS_ASSERT_THROWS(testee.connect(afl::net::Name::parse("141.30.6.1:8080", "foo"), 10), afl::except::FileProblemException);
}

/** Test connect(), server closing connection (this is what ssh does). */
void
TestNetTunnelSocks4NetworkStack::testConnectClose()
{
    afl::test::NetworkStack mock("testConnectClose");
    afl::net::tunnel::Socks4NetworkStack testee(mock, afl::net::Name("s4host", "s4port"));

    // Expect a connection that wants to connect to 141.30.6.1:8080.
    // Server rejects by closing the connection.
    static const uint8_t TX[] = {4,1,0x1F,0x90,141,30,6,1,0};
    mock.expectConnection(afl::net::Name("s4host", "s4port"), afl::base::Nothing, TX);

    // Connect
    TS_ASSERT_THROWS(testee.connect(afl::net::Name::parse("141.30.6.1:8080", "foo"), 10), afl::except::FileProblemException);
}

/** Test listen(). */
void
TestNetTunnelSocks4NetworkStack::testListen()
{
    afl::test::NetworkStack mock("testListen");
    afl::net::tunnel::Socks4NetworkStack testee(mock, afl::net::Name("s4host", "s4port"));

    // Expect a connection that wants to listen on 127.0.0.1:2000
    static const uint8_t TX[] = {4,2,7,0xD0,127,0,0,1,0,'a','b'};
    static const uint8_t RX[] = {0,90,7,0xD0,127,0,0,1,
                                 0,90,5,6,192,168,3,4,
                                 'x','y'};
    mock.expectConnection(afl::net::Name("s4host", "s4port"), RX, TX);

    // Connect
    afl::base::Ref<afl::net::Listener> lis(testee.listen(afl::net::Name("127.0.0.1", 2000), 10));
    afl::base::Ptr<afl::net::Socket> sock(lis->accept());
    TS_ASSERT(sock.get() != 0);
    TS_ASSERT_EQUALS(sock->getName(), "127.0.0.1:2000");
    TS_ASSERT_EQUALS(sock->getPeerName().toString(), "192.168.3.4:1286");
    verifyFinish(*sock);
}

/** Test listen() failure: server closes connection. */
void
TestNetTunnelSocks4NetworkStack::testListenFail()
{
    afl::test::NetworkStack mock("testListenFail");
    afl::net::tunnel::Socks4NetworkStack testee(mock, afl::net::Name("s4host", "s4port"));

    // Expect a connection that wants to listen on 127.0.0.1:2000
    static const uint8_t TX[] = {4,2,7,0xD0,127,0,0,1,0,'a','b'};
    mock.expectConnection(afl::net::Name("s4host", "s4port"), afl::base::Nothing, TX);

    // Connect
    afl::base::Ref<afl::net::Listener> lis(testee.listen(afl::net::Name("127.0.0.1", 2000), 10));
    TS_ASSERT_THROWS(lis->accept(), afl::except::FileProblemException);
}

/** Test listen() failure: server closes connection.
    Because a timeout was used, no exception. */
void
TestNetTunnelSocks4NetworkStack::testListenFail2()
{
    afl::test::NetworkStack mock("testListenFail2");
    afl::net::tunnel::Socks4NetworkStack testee(mock, afl::net::Name("s4host", "s4port"));

    // Expect a connection that wants to listen on 127.0.0.1:2000
    static const uint8_t TX[] = {4,2,7,0xD0,127,0,0,1,0,'a','b'};
    mock.expectConnection(afl::net::Name("s4host", "s4port"), afl::base::Nothing, TX);

    // Connect
    afl::base::Ref<afl::net::Listener> lis(testee.listen(afl::net::Name("127.0.0.1", 2000), 10));
    afl::base::Ptr<afl::net::Socket> sock(lis->accept(1000));
    TS_ASSERT(sock.get() == 0);
}

/** Test listen() failure: server not reachable. */
void
TestNetTunnelSocks4NetworkStack::testListenFailConnect()
{
    afl::net::NullNetworkStack nn;
    afl::net::tunnel::Socks4NetworkStack testee(nn, afl::net::Name("s4host", "s4port"));

    // Connect
    afl::base::Ref<afl::net::Listener> lis(testee.listen(afl::net::Name("127.0.0.1", 2000), 10));
    TS_ASSERT_THROWS(lis->accept(1000), afl::except::FileProblemException);
}

/** Test listen(), asynchronous. */
void
TestNetTunnelSocks4NetworkStack::testListenAsync()
{
    afl::test::NetworkStack mock("testListenAsync");
    afl::net::tunnel::Socks4NetworkStack testee(mock, afl::net::Name("s4host", "s4port"));

    // Expect a connection that wants to listen on 127.0.0.1:2000
    static const uint8_t TX[] = {4,2,7,0xD0,127,0,0,1,0,'a','b'};
    static const uint8_t RX[] = {0,90,7,0xD0,127,0,0,1,
                                 0,90,5,6,192,168,3,4,
                                 'x','y'};
    mock.expectConnection(afl::net::Name("s4host", "s4port"), RX, TX);

    // Listen
    afl::base::Ref<afl::net::Listener> lis(testee.listen(afl::net::Name("127.0.0.1", 2000), 10));
    afl::async::Controller ctl;
    afl::net::AcceptOperation op;
    lis->acceptAsync(ctl, op);

    // Wait
    afl::async::Operation* resultOp = ctl.wait();
    TS_ASSERT_EQUALS(resultOp, &op);
    TS_ASSERT(op.getResult().get() != 0);
    verifyFinish(*op.getResult());
}


/** Test listen(), cancellation. */
void
TestNetTunnelSocks4NetworkStack::testListenCancel()
{
    afl::test::NetworkStack mock("testListenCancel");
    afl::net::tunnel::Socks4NetworkStack testee(mock, afl::net::Name("s4host", "s4port"));

    // Expect a connection that wants to listen on 127.0.0.1:2000
    static const uint8_t TX[] = {4,2,7,0xD0,127,0,0,1,0,'a','b'};
    static const uint8_t RX[] = {0,90,7,0xD0,127,0,0,1,
                                 0,90,5,6,192,168,3,4,
                                 'x','y'};
    mock.expectConnection(afl::net::Name("s4host", "s4port"), RX, TX);

    // Listen
    afl::base::Ref<afl::net::Listener> lis(testee.listen(afl::net::Name("127.0.0.1", 2000), 10));
    afl::async::Controller ctl;
    afl::net::AcceptOperation op;
    lis->acceptAsync(ctl, op);

    // Cancel
    lis->cancel(ctl, op);

    // Verify
    TS_ASSERT(ctl.wait(100) == 0);
}

/** Test listen(), failure to send. */
void
TestNetTunnelSocks4NetworkStack::testListenFailSend()
{
    afl::test::NetworkStack mock("testListenFailSend");
    afl::net::tunnel::Socks4NetworkStack testee(mock, afl::net::Name("s4host", "s4port"));

    // Expect a connection, but don't allow it to send
    mock.expectConnection(afl::net::Name("s4host", "s4port"), afl::base::Nothing, afl::base::Nothing);

    // Listen
    afl::base::Ref<afl::net::Listener> lis(testee.listen(afl::net::Name("127.0.0.1", 2000), 10));
    afl::base::Ptr<afl::net::Socket> sock(lis->accept(1000));
    TS_ASSERT(sock.get() == 0);
}

/** Test listen(), syntax error. */
void
TestNetTunnelSocks4NetworkStack::testListenFailSyntax()
{
    afl::test::NetworkStack mock("testListenFailSyntax");
    afl::net::tunnel::Socks4NetworkStack testee(mock, afl::net::Name("s4host", "s4port"));

    // Expect a connection that wants to listen on 127.0.0.1:2000
    mock.expectConnection(afl::net::Name("s4host", "s4port"), afl::base::Nothing, afl::base::Nothing);

    // Connect
    afl::base::Ref<afl::net::Listener> lis(testee.listen(afl::net::Name("127.0.0.1", "xyzzy"), 10));
    afl::base::Ptr<afl::net::Socket> sock(lis->accept(1000));
    TS_ASSERT(sock.get() == 0);
}

