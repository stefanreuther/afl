/**
  *  \file u/t_checksums.hpp
  *  \brief Test for afl::checksums
  */
#ifndef AFL_U_T_CHECKSUMS_HPP
#define AFL_U_T_CHECKSUMS_HPP

#include <cxxtest/TestSuite.h>

class TestChecksumsAdler32 : public CxxTest::TestSuite {
 public:
    void testIt();
    void testInterface();
};

class TestChecksumsByteSum : public CxxTest::TestSuite {
 public:
    void testIt();
    void testInterface();
};

class TestChecksumsCRC16 : public CxxTest::TestSuite {
 public:
    void testIt();
    void testInterface();
};

class TestChecksumsCRC32 : public CxxTest::TestSuite {
 public:
    void testIt();
    void testInterface();
};

class TestChecksumsChecksum : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestChecksumsHash : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestChecksumsMD5 : public CxxTest::TestSuite {
 public:
    void testBasic();
    void testSuite();
    void testSplit();
    void testSpeed();
    void testHMAC();
};

class TestChecksumsSHA1 : public CxxTest::TestSuite {
 public:
    void testBasic();
    void testSuite();
    void testSpeed();
    void testPBKDF2();
};

#endif
