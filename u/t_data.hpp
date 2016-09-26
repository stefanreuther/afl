/**
  *  \file u/t_data.hpp
  *  \brief Test for afl::data
  */
#ifndef AFL_U_T_DATA_HPP
#define AFL_U_T_DATA_HPP

#include <cxxtest/TestSuite.h>

class TestDataAccess : public CxxTest::TestSuite {
 public:
    void testSimple();
    void testVector();
    void testHash();
    void testError();
    void testList();
};

class TestDataBooleanValue : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestDataDefaultValueFactory : public CxxTest::TestSuite {
 public:
    void testString();
    void testInteger();
    void testFloat();
    void testBoolean();
    void testHash();
    void testVector();
    void testError();
    void testNull();
};

class TestDataErrorValue : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestDataFloatValue : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestDataNameMap : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestDataNameQuery : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestDataHash : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestDataHashValue : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestDataIntegerList : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestDataIntegerValue : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestDataScalarValue : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestDataSegment : public CxxTest::TestSuite {
 public:
    void testIndex();
    void testStack();
    void testPush();
    void testInsert();
};

class TestDataSegmentView : public CxxTest::TestSuite {
 public:
    void testEat();
    void testTypes();
};

class TestDataStringList : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestDataStringValue : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestDataValue : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestDataValueFactory : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestDataVector : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestDataVectorValue : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestDataVisitor : public CxxTest::TestSuite {
 public:
    void testIt();
};

#endif
