/**
  *  \file u/t_except.hpp
  *  \brief Test for afl::except
  */
#ifndef AFL_U_T_EXCEPT_HPP
#define AFL_U_T_EXCEPT_HPP

#include <cxxtest/TestSuite.h>

class TestExcept : public CxxTest::TestSuite {
    // It makes little sense to split this into individual classes,
    // so let's do it all on one.
 public:
    void testFileFormatException();
    void testFileProblemException();
    void testFileSystemException();
    void testFileTooShortException();
    void testInvalidDataException();
    void testUnsupportedException();
    void testSystemException();
    void testRemoteErrorException();
};

class TestExceptAssertionFailedException : public CxxTest::TestSuite {
 public:
    void testClass();
    void testCheck();
};

#endif
