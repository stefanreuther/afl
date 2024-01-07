/**
  *  \file test/afl/net/tunnel/socks4networkstacktest.cpp
  *  \brief Test for afl::net::tunnel::Socks4NetworkStack
  */

#include "afl/net/tunnel/socks4networkstack.hpp"

#include "afl/async/controller.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/net/acceptoperation.hpp"
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
AFL_TEST("afl.net.tunnel.Socks4NetworkStack:connect:ip", a)
{
    afl::test::NetworkStack mock("testConnect");
    afl::net::tunnel::Socks4NetworkStack testee(mock, afl::net::Name("s4host", "s4port"));

    // Expect a connection that wants to connect to 141.30.6.1:8080
    static const uint8_t TX[] = {4,1,0x1F,0x90,141,30,6,1,0,'a','b'};
    static const uint8_t RX[] = {0,90,77,77,77,77,77,77,'x','y'};
    mock.expectConnection(afl::net::Name("s4host", "s4port"), RX, TX);

    // Connect
    afl::base::Ref<afl::net::Socket> sock(testee.connect(afl::net::Name::parse("141.30.6.1:8080", "foo"), 10));
    verifyFinish(a, *sock);
}

/** Test connect(), domain name case.
    This must use the SOCKS4a format with an invalid IP address. */
AFL_TEST("afl.net.tunnel.Socks4NetworkStack:connect:domain", a)
{
    afl::test::NetworkStack mock("testConnectDomain");
    afl::net::tunnel::Socks4NetworkStack testee(mock, afl::net::Name("s4host", "s4port"));

    // Expect a connection that wants to connect to domain:77
    static const uint8_t TX[] = {4,1,0,77,0,0,0,1,0,'d','o','m','a','i','n',0,'a','b'};
    static const uint8_t RX[] = {0,90,77,77,77,77,77,77,'x','y'};
    mock.expectConnection(afl::net::Name("s4host", "s4port"), RX, TX);

    // Connect
    afl::base::Ref<afl::net::Socket> sock(testee.connect(afl::net::Name::parse("domain", "77"), 10));
    verifyFinish(a, *sock);
}

/** Test connect(), bad IP case.
    If the SOCKS4 format were used, the IP would have been mistaken as SOCKS4a format, so this must use SOCKS4a. */
AFL_TEST("afl.net.tunnel.Socks4NetworkStack:connect:bad-ip", a)
{
    afl::test::NetworkStack mock("testConnectBadIP");
    afl::net::tunnel::Socks4NetworkStack testee(mock, afl::net::Name("s4host", "s4port"));

    // Expect a connection that wants to connect to 0.0.0.42:23
    static const uint8_t TX[] = {4,1,0,23,0,0,0,1,0,'0','.','0','.','0','.','4','2',0,'a','b'};
    static const uint8_t RX[] = {0,90,77,77,77,77,77,77,'x','y'};
    mock.expectConnection(afl::net::Name("s4host", "s4port"), RX, TX);

    // Connect
    afl::base::Ref<afl::net::Socket> sock(testee.connect(afl::net::Name::parse("0.0.0.42:23", "77"), 10));
    verifyFinish(a, *sock);
}

/** Test connect(), invalid case. */
AFL_TEST("afl.net.tunnel.Socks4NetworkStack:connect:invalid-name", a)
{
    afl::test::NetworkStack mock("testConnectInvalid");
    afl::net::tunnel::Socks4NetworkStack testee(mock, afl::net::Name("s4host", "s4port"));

    // FIXME: This will make a connection, but not transfer any data
    mock.expectConnection(afl::net::Name("s4host", "s4port"), afl::base::Nothing, afl::base::Nothing);

    AFL_CHECK_THROWS(a("connect"), testee.connect(afl::net::Name("1.2.3.4", "huhu"), 10), afl::except::FileProblemException);
}

/** Test connect(), rejection by server. */
AFL_TEST("afl.net.tunnel.Socks4NetworkStack:connect:server-rejects", a)
{
    afl::test::NetworkStack mock("testConnectReject");
    afl::net::tunnel::Socks4NetworkStack testee(mock, afl::net::Name("s4host", "s4port"));

    // Expect a connection that wants to connect to 141.30.6.1:8080.
    // Server rejects.
    static const uint8_t TX[] = {4,1,0x1F,0x90,141,30,6,1,0};
    static const uint8_t RX[] = {0,91,77,77,77,77,77,77};
    mock.expectConnection(afl::net::Name("s4host", "s4port"), RX, TX);

    // Connect
    AFL_CHECK_THROWS(a("connect"), testee.connect(afl::net::Name::parse("141.30.6.1:8080", "foo"), 10), afl::except::FileProblemException);
}

/** Test connect(), server closing connection (this is what ssh does). */
AFL_TEST("afl.net.tunnel.Socks4NetworkStack:connect:server-closes", a)
{
    afl::test::NetworkStack mock("testConnectClose");
    afl::net::tunnel::Socks4NetworkStack testee(mock, afl::net::Name("s4host", "s4port"));

    // Expect a connection that wants to connect to 141.30.6.1:8080.
    // Server rejects by closing the connection.
    static const uint8_t TX[] = {4,1,0x1F,0x90,141,30,6,1,0};
    mock.expectConnection(afl::net::Name("s4host", "s4port"), afl::base::Nothing, TX);

    // Connect
    AFL_CHECK_THROWS(a("connect"), testee.connect(afl::net::Name::parse("141.30.6.1:8080", "foo"), 10), afl::except::FileProblemException);
}

/** Test listen(). */
AFL_TEST("afl.net.tunnel.Socks4NetworkStack:listen:ip", a)
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
    a.checkNonNull("socket exists", sock.get());
    a.checkEqual("socket getName", sock->getName(), "127.0.0.1:2000");
    a.checkEqual("socket getPeerName", sock->getPeerName().toString(), "192.168.3.4:1286");
    verifyFinish(a, *sock);
}

/** Test listen() failure: server closes connection. */
AFL_TEST("afl.net.tunnel.Socks4NetworkStack:listen:server-close", a)
{
    afl::test::NetworkStack mock("testListenFail");
    afl::net::tunnel::Socks4NetworkStack testee(mock, afl::net::Name("s4host", "s4port"));

    // Expect a connection that wants to listen on 127.0.0.1:2000
    static const uint8_t TX[] = {4,2,7,0xD0,127,0,0,1,0,'a','b'};
    mock.expectConnection(afl::net::Name("s4host", "s4port"), afl::base::Nothing, TX);

    // Connect
    afl::base::Ref<afl::net::Listener> lis(testee.listen(afl::net::Name("127.0.0.1", 2000), 10));
    AFL_CHECK_THROWS(a("accept"), lis->accept(), afl::except::FileProblemException);
}

/** Test listen() failure: server closes connection.
    Because a timeout was used, no exception. */
AFL_TEST("afl.net.tunnel.Socks4NetworkStack:listen:server-close:timeout", a)
{
    afl::test::NetworkStack mock("testListenFail2");
    afl::net::tunnel::Socks4NetworkStack testee(mock, afl::net::Name("s4host", "s4port"));

    // Expect a connection that wants to listen on 127.0.0.1:2000
    static const uint8_t TX[] = {4,2,7,0xD0,127,0,0,1,0,'a','b'};
    mock.expectConnection(afl::net::Name("s4host", "s4port"), afl::base::Nothing, TX);

    // Connect
    afl::base::Ref<afl::net::Listener> lis(testee.listen(afl::net::Name("127.0.0.1", 2000), 10));
    afl::base::Ptr<afl::net::Socket> sock(lis->accept(1000));
    a.checkNull("socket", sock.get());
}

/** Test listen() failure: server not reachable. */
AFL_TEST("afl.net.tunnel.Socks4NetworkStack:listen:server-unreachable", a)
{
    afl::net::NullNetworkStack nn;
    afl::net::tunnel::Socks4NetworkStack testee(nn, afl::net::Name("s4host", "s4port"));

    // Connect
    afl::base::Ref<afl::net::Listener> lis(testee.listen(afl::net::Name("127.0.0.1", 2000), 10));
    AFL_CHECK_THROWS(a("accept"), lis->accept(1000), afl::except::FileProblemException);
}

/** Test listen(), asynchronous. */
AFL_TEST("afl.net.tunnel.Socks4NetworkStack:acceptAsync", a)
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
    a.checkEqual("result", resultOp, &op);
    a.check("result socket", op.getResult().get());
    verifyFinish(a, *op.getResult());
}

/** Test listen(), cancellation. */
AFL_TEST("afl.net.tunnel.Socks4NetworkStack:acceptAsync:cancel", a)
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
    a.checkNull("timeout", ctl.wait(100));
}

/** Test listen(), failure to send. */
AFL_TEST("afl.net.tunnel.Socks4NetworkStack:listen:send-failure", a)
{
    afl::test::NetworkStack mock("testListenFailSend");
    afl::net::tunnel::Socks4NetworkStack testee(mock, afl::net::Name("s4host", "s4port"));

    // Expect a connection, but don't allow it to send
    mock.expectConnection(afl::net::Name("s4host", "s4port"), afl::base::Nothing, afl::base::Nothing);

    // Listen
    afl::base::Ref<afl::net::Listener> lis(testee.listen(afl::net::Name("127.0.0.1", 2000), 10));
    afl::base::Ptr<afl::net::Socket> sock(lis->accept(1000));
    a.checkNull("socket", sock.get());
}

/** Test listen(), syntax error. */
AFL_TEST("afl.net.tunnel.Socks4NetworkStack:listen:invalid-name", a)
{
    afl::test::NetworkStack mock("testListenFailSyntax");
    afl::net::tunnel::Socks4NetworkStack testee(mock, afl::net::Name("s4host", "s4port"));

    // Expect a connection that wants to listen on 127.0.0.1:2000
    mock.expectConnection(afl::net::Name("s4host", "s4port"), afl::base::Nothing, afl::base::Nothing);

    // Connect
    afl::base::Ref<afl::net::Listener> lis(testee.listen(afl::net::Name("127.0.0.1", "xyzzy"), 10));
    afl::base::Ptr<afl::net::Socket> sock(lis->accept(1000));
    a.checkNull("socket", sock.get());
}
