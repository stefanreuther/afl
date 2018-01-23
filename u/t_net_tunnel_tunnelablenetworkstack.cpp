/**
  *  \file u/t_net_tunnel_tunnelablenetworkstack.cpp
  *  \brief Test for afl::net::tunnel::TunnelableNetworkStack
  */

#include "afl/net/tunnel/tunnelablenetworkstack.hpp"

#include "t_net_tunnel.hpp"
#include "afl/async/controller.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/except/unsupportedexception.hpp"
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

/** Test default (pass-through) behaviour. */
void
TestNetTunnelTunnelableNetworkStack::testDefault()
{
    afl::test::NetworkStack mock("testDefault");
    afl::net::tunnel::TunnelableNetworkStack testee(mock);

    // Expect connection
    static const uint8_t TX[] = {'a','b'};
    static const uint8_t RX[] = {'x','y'};
    mock.expectConnection(afl::net::Name("a", "b"), RX, TX);

    // Connect
    afl::base::Ref<afl::net::Socket> sock(testee.connect(afl::net::Name("a", "b"), 10));
    verifyFinish(*sock);
}

/** Test SOCKS4 proxy. */
void
TestNetTunnelTunnelableNetworkStack::testSocks4()
{
    afl::test::NetworkStack mock("testSocks4");
    afl::net::tunnel::TunnelableNetworkStack testee(mock);
    TS_ASSERT(testee.add("socks4://sh"));

    // Expect connection
    static const uint8_t TX[] = {4,1,0,80,85,214,218,199,0,'a','b'};
    static const uint8_t RX[] = {0,90,77,77,77,77,77,77,'x','y'};
    mock.expectConnection(afl::net::Name("sh", "1080"), RX, TX);

    // Connect
    afl::base::Ref<afl::net::Socket> sock(testee.connect(afl::net::Name("85.214.218.199", 80), 10));
    verifyFinish(*sock);
}

/** Test SOCKS5 proxy. */
void
TestNetTunnelTunnelableNetworkStack::testSocks5()
{
    afl::test::NetworkStack mock("testSocks5");
    afl::net::tunnel::TunnelableNetworkStack testee(mock);
    TS_ASSERT(testee.add("socks5://s5h:3579"));

    // Expect connection
    static const uint8_t TX[] = {5,1,0, 5,1,0,1,141,30,6,1,0x1F,0x90,'a','b'};
    static const uint8_t RX[] = {5,0, 5,0,0,1,7,6,5,4,99,22,'x','y'};
    mock.expectConnection(afl::net::Name("s5h", "3579"), RX, TX);

    // Connect
    afl::base::Ref<afl::net::Socket> sock(testee.connect(afl::net::Name("141.30.6.1", 8080), 10));
    verifyFinish(*sock);
}

/** Test chaining multiple tunnels. */
void
TestNetTunnelTunnelableNetworkStack::testChain()
{
    afl::test::NetworkStack mock("testChain");
    afl::net::tunnel::TunnelableNetworkStack testee(mock);
    TS_ASSERT(testee.add("socks4://s4h:40"));
    TS_ASSERT(testee.add("socks5://s5h"));

    // Expect connection
    static const uint8_t TX[] = {4,1,0x04,0x38,0,0,0,1,0,'s','5','h',0,
                                 5,1,0, 5,1,0,1,141,30,6,1,0x1F,0x90,
                                 'a','b'};
    static const uint8_t RX[] = {0,90,66,66,66,66,66,66,
                                 5,0, 5,0,0,1,7,6,5,4,99,22,
                                 'x','y'};
    mock.expectConnection(afl::net::Name("s4h", "40"), RX, TX);

    // Connect
    afl::base::Ref<afl::net::Socket> sock(testee.connect(afl::net::Name("141.30.6.1", 8080), 10));
    verifyFinish(*sock);
}

/** Test reset(). */
void
TestNetTunnelTunnelableNetworkStack::testReset()
{
    afl::test::NetworkStack mock("testReset");
    afl::net::tunnel::TunnelableNetworkStack testee(mock);

    // Add tunnels and drop them again
    TS_ASSERT(testee.add("socks4://s4h:40"));
    TS_ASSERT(testee.add("socks5://s5h"));
    testee.reset();

    // Expect connection
    static const uint8_t TX[] = {'a','b'};
    static const uint8_t RX[] = {'x','y'};
    mock.expectConnection(afl::net::Name("a", "b"), RX, TX);

    // Connect
    afl::base::Ref<afl::net::Socket> sock(testee.connect(afl::net::Name("a", "b"), 10));
    verifyFinish(*sock);
}

/** Test failure to add. */
void
TestNetTunnelTunnelableNetworkStack::testFail()
{
    afl::test::NetworkStack mock("testFail");
    afl::net::tunnel::TunnelableNetworkStack testee(mock);

    TS_ASSERT(!testee.add("127.0.0.1"));
    TS_ASSERT(!testee.add("ftp://x.y/"));
}

