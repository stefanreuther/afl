/**
  *  \file u/t_io_resp.hpp
  *  \brief Test for afl::io::resp
  */
#ifndef AFL_U_T_IO_RESP_HPP
#define AFL_U_T_IO_RESP_HPP

#include <cxxtest/TestSuite.h>

class TestIoRespParser : public CxxTest::TestSuite {
 public:
    void testIntegers();
    void testString();
    void testVector();
    void testOneliners();
    void testShortForm();
};

class TestIoRespWriter : public CxxTest::TestSuite {
 public:
    void testSimple();
    void testNonObj();
};

#endif
