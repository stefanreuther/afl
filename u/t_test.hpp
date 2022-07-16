/**
  *  \file u/t_test.hpp
  *  \brief Tests for afl::test
  */
#ifndef AFL_U_T_TEST_HPP
#define AFL_U_T_TEST_HPP

#include <cxxtest/TestSuite.h>

class TestTestAssert : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestTestCallReceiver : public CxxTest::TestSuite {
 public:
    void testSuccess();
    void testFailUnexpected();
    void testFailDifferent();
    void testFailNoReturn();
    void testFailWrongRetun();
    void testFailOutstanding();
    void testFailOutstandingReturn();
};

class TestTestCommandHandler : public CxxTest::TestSuite {
 public:
    void testSuccess();
    void testFailWrongCommand();
    void testFailNoCommand();
    void testFailNoResult();
    void testFailOutstandingCommand();
    void testFailOutstandingResult();
    void testParameterTypes();
};

class TestTestLogListener : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestTestNetworkStack : public CxxTest::TestSuite {
 public:
    void testSuccess();
    void testFailure();
    void testFailureWrongTarget();
    void testFailureWrongData();
};

class TestTestSocket : public CxxTest::TestSuite {
 public:
    void testSuccess();
    void testFailure();
};

class TestTestTranslator : public CxxTest::TestSuite {
 public:
    void testIt();
};

#endif
