/**
  *  \file test/afl/net/tunnel/tunnelablenetworkstacktest.cpp
  *  \brief Test for afl::net::tunnel::TunnelableNetworkStack
  */

#include "afl/net/tunnel/tunnelablenetworkstack.hpp"

#include "afl/async/controller.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/except/unsupportedexception.hpp"
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

/** Test default (pass-through) behaviour. */
AFL_TEST("afl.net.tunnel.TunnelableNetworkStack:default", a)
{
    afl::test::NetworkStack mock("testDefault");
    afl::net::tunnel::TunnelableNetworkStack testee(mock);

    // Expect connection
    static const uint8_t TX[] = {'a','b'};
    static const uint8_t RX[] = {'x','y'};
    mock.expectConnection(afl::net::Name("a", "b"), RX, TX);

    // Connect
    afl::base::Ref<afl::net::Socket> sock(testee.connect(afl::net::Name("a", "b"), 10));
    verifyFinish(a, *sock);
}

/** Test SOCKS4 proxy. */
AFL_TEST("afl.net.tunnel.TunnelableNetworkStack:socks4", a)
{
    afl::test::NetworkStack mock("testSocks4");
    afl::net::tunnel::TunnelableNetworkStack testee(mock);
    a.check("add", testee.add("socks4://sh"));

    // Expect connection
    static const uint8_t TX[] = {4,1,0,80,85,214,218,199,0,'a','b'};
    static const uint8_t RX[] = {0,90,77,77,77,77,77,77,'x','y'};
    mock.expectConnection(afl::net::Name("sh", "1080"), RX, TX);

    // Connect
    afl::base::Ref<afl::net::Socket> sock(testee.connect(afl::net::Name("85.214.218.199", 80), 10));
    verifyFinish(a, *sock);
}

/** Test SOCKS5 proxy. */
AFL_TEST("afl.net.tunnel.TunnelableNetworkStack:socks5", a)
{
    afl::test::NetworkStack mock("testSocks5");
    afl::net::tunnel::TunnelableNetworkStack testee(mock);
    a.check("add", testee.add("socks5://s5h:3579"));

    // Expect connection
    static const uint8_t TX[] = {5,1,0, 5,1,0,1,141,30,6,1,0x1F,0x90,'a','b'};
    static const uint8_t RX[] = {5,0, 5,0,0,1,7,6,5,4,99,22,'x','y'};
    mock.expectConnection(afl::net::Name("s5h", "3579"), RX, TX);

    // Connect
    afl::base::Ref<afl::net::Socket> sock(testee.connect(afl::net::Name("141.30.6.1", 8080), 10));
    verifyFinish(a, *sock);
}

/** Test chaining multiple tunnels. */
AFL_TEST("afl.net.tunnel.TunnelableNetworkStack:chain", a)
{
    afl::test::NetworkStack mock("testChain");
    afl::net::tunnel::TunnelableNetworkStack testee(mock);
    a.check("add socks4", testee.add("socks4://s4h:40"));
    a.check("add socks5", testee.add("socks5://s5h"));

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
    verifyFinish(a, *sock);
}

/** Test reset(). */
AFL_TEST("afl.net.tunnel.TunnelableNetworkStack:reset", a)
{
    afl::test::NetworkStack mock("testReset");
    afl::net::tunnel::TunnelableNetworkStack testee(mock);

    // Add tunnels and drop them again
    a.check("add socks4", testee.add("socks4://s4h:40"));
    a.check("add socks5", testee.add("socks5://s5h"));
    testee.reset();

    // Expect connection
    static const uint8_t TX[] = {'a','b'};
    static const uint8_t RX[] = {'x','y'};
    mock.expectConnection(afl::net::Name("a", "b"), RX, TX);

    // Connect
    afl::base::Ref<afl::net::Socket> sock(testee.connect(afl::net::Name("a", "b"), 10));
    verifyFinish(a, *sock);
}

/** Test failure to add. */
AFL_TEST("afl.net.tunnel.TunnelableNetworkStack:add:invalid", a)
{
    afl::test::NetworkStack mock("testFail");
    afl::net::tunnel::TunnelableNetworkStack testee(mock);

    a.check("add 1", !testee.add("127.0.0.1"));
    a.check("add 2", !testee.add("ftp://x.y/"));
}
