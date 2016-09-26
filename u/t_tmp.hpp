/**
  *  \file u/t_tmp.hpp
  *  \brief Test for afl::tmp
  */
#ifndef AFL_U_T_TMP_HPP
#define AFL_U_T_TMP_HPP

#include <cxxtest/TestSuite.h>

class TestTmpCopyCV : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestTmpDereferenceType : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestTmpIfThenElse : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestTmpIsSameType : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestTmpSizedInt : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestTmpStripCV : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestTmpSizedUInt : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestTmpTypeList : public CxxTest::TestSuite {
 public:
    void testIt();
};

#endif
