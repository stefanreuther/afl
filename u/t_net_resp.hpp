/**
  *  \file u/t_net_resp.hpp
  *  \brief Test for afl::net::resp
  */
#ifndef AFL_U_T_NET_RESP_HPP
#define AFL_U_T_NET_RESP_HPP

#include <cxxtest/TestSuite.h>

class TestNetRespClient : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestNetRespProtocolHandler : public CxxTest::TestSuite {
 public:
    void testIt();
};

#endif
