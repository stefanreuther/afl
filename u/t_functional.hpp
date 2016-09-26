/**
  *  \file u/t_functional.hpp
  *  \brief Test for afl::functional
  */
#ifndef AFL_U_T_FUNCTIONAL_HPP
#define AFL_U_T_FUNCTIONAL_HPP

#include <cxxtest/TestSuite.h>

class TestFunctionalBinaryFunction : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestFunctionalMapping : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestFunctionalStringTable : public CxxTest::TestSuite {
 public:
    void testStaticConst();
    void testStaticMutable();
    void testStringArray();
    void testStringVector();
    void testEmpty();
};

class TestFunctionalUnaryFunction : public CxxTest::TestSuite {
 public:
    void testIt();
    void testOptional();
};

#endif
