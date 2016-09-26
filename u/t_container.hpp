/**
  *  \file u/t_container.hpp
  *  \brief Test for afl::container
  */
#ifndef AFL_U_T_CONTAINER_HPP
#define AFL_U_T_CONTAINER_HPP

#include <cxxtest/TestSuite.h>

class TestContainerDereferencingIterator : public CxxTest::TestSuite {
 public:
    void testIt();
};


class TestContainerMemberIterator : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestContainerPtrMap : public CxxTest::TestSuite {
 public:
    void testIt();
    void testFind();
    void testConst();
};

class TestContainerPtrMultiListBase : public CxxTest::TestSuite {
 public:
    void testBasic();
    void testBasicErase1();
    void testBasicErase2();
    void testBasicErase3();
    void testBasicMultiIter();
    void testBasicInsert1();
    void testBasicInsert2();
    void testBasicInsert3();
    void testBasicInsert4();
};

class TestContainerPtrMultiList : public CxxTest::TestSuite {
 public:
    void testTyped();
    void testTypedMerge();
    void testTypedSort();
};

class TestContainerPtrQueue : public CxxTest::TestSuite {
 public:
    void testIt();
    void testConst();
};

class TestContainerPtrVector : public CxxTest::TestSuite {
 public:
    void testIt();
    void testConst();
    void testSort1();
    void testSort2();
    void testSort3();
    void testSort4();
};

#endif
