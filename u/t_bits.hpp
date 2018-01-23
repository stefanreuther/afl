/**
  *  \file u/t_bits.hpp
  *  \brief Test for afl::bits
  */
#ifndef AFL_U_T_BITS_HPP
#define AFL_U_T_BITS_HPP

#include <cxxtest/TestSuite.h>

class TestBitsBits : public CxxTest::TestSuite {
 public:
    void testPower();
    void testLog();
    void testReverse();
    void testBitPop();
};

class TestBitsFixedString : public CxxTest::TestSuite {
 public:
    void testUnpack();
    void testPack();
    void testClass();
};

class TestBitsInt8 : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestBitsInt16BE : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestBitsInt16LE : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestBitsInt32BE : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestBitsInt32LE : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestBitsInt64BE : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestBitsInt64LE : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestBitsPack : public CxxTest::TestSuite {
 public:
    void testPackArray();
    void testUnpackArray();
};

class TestBitsRotate : public CxxTest::TestSuite {
 public:
    void test8();
    void test16();
    void test32();
    void test64();
};

class TestBitsSmallSet : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestBitsUInt8 : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestBitsUInt16BE : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestBitsUInt16LE : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestBitsUInt32BE : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestBitsUInt32LE : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestBitsUInt64BE : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestBitsUInt64LE : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestBitsValue : public CxxTest::TestSuite {
 public:
    void testExpr();
    void testSignedTypes();
    void testUnsignedTypes();
    void testStructure();
};

#endif
