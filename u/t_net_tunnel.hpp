/**
  *  \file u/t_net_tunnel.hpp
  *  \brief Tests for afl::net::tunnel
  */
#ifndef AFL_U_T_NET_TUNNEL_HPP
#define AFL_U_T_NET_TUNNEL_HPP

#include <cxxtest/TestSuite.h>

class TestNetTunnelSocketWrapper : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestNetTunnelSocks4NetworkStack : public CxxTest::TestSuite {
 public:
    void testConnect();
    void testConnectDomain();
    void testConnectBadIP();
    void testConnectInvalid();
    void testConnectReject();
    void testConnectClose();
    void testListen();
    void testListenFail();
    void testListenFail2();
    void testListenFailConnect();
    void testListenAsync();
    void testListenCancel();
    void testListenFailSend();
    void testListenFailSyntax();
};

class TestNetTunnelSocks5NetworkStack : public CxxTest::TestSuite {
 public:
    void testConnect();
    void testConnectDomain();
    void testConnectInvalid();
    void testConnectReject();
    void testConnectClose();
    void testConnectAuthMismatch();
    void testConnectAuthClose();
    void testConnectUnreachable();
    void testListen();
};

class TestNetTunnelTunnelableNetworkStack : public CxxTest::TestSuite {
 public:
    void testDefault();
    void testSocks4();
    void testSocks5();
    void testChain();
    void testReset();
    void testFail();
};

#endif
