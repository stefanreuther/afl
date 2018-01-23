/**
  *  \file u/t_net_smtp.hpp
  *  \brief Tests for afl::net::smtp
  */
#ifndef AFL_U_T_NET_SMTP_HPP
#define AFL_U_T_NET_SMTP_HPP

#include <cxxtest/TestSuite.h>

class TestNetSmtpConfiguration : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestNetSmtpMailRequest : public CxxTest::TestSuite {
 public:
    void testSimple();
    void testNoReceiver();
    void testVariant();
    void testClose();
    void testMultilineGreeting();
    void testPrematureClose();
    void testErrorResponse();
};

#endif
