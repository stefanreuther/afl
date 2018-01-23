/**
  *  \file u/t_net_line.hpp
  *  \brief Tests for afl::net::line
  */
#ifndef AFL_U_T_NET_LINE_HPP
#define AFL_U_T_NET_LINE_HPP

#include <cxxtest/TestSuite.h>

class TestNetLineClient : public CxxTest::TestSuite {
 public:
    void testSimpleQuery();
};

class TestNetLineLineHandler : public CxxTest::TestSuite {
 public:
    void testInterface();
};

class TestNetLineLineSink : public CxxTest::TestSuite {
 public:
    void testInterface();
};

class TestNetLineProtocolHandler : public CxxTest::TestSuite {
 public:
    void testSimpleQuery();
};

class TestNetLineSimpleQuery : public CxxTest::TestSuite {
 public:
    void testIt();
};

#endif
