/**
  *  \file u/t_io_json.hpp
  *  \brief Test for afl::io::json
  */
#ifndef AFL_U_T_IO_JSON_HPP
#define AFL_U_T_IO_JSON_HPP

#include <cxxtest/TestSuite.h>

class TestIoJsonParser : public CxxTest::TestSuite {
 public:
    void testKeywords();
    void testErrors();

    void testString();
    void testInteger();
    void testFloat();
    void testHash();
    void testVector();

    void testTestsuiteGood();
    void testTestsuiteBad();
    void testTestsuiteUgly();
};

class TestIoJsonWriter : public CxxTest::TestSuite {
 public:
    void testSimple();
    void testFormat();
};

#endif
