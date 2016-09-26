/**
  *  \file u/t_io.hpp
  *  \brief Test for afl::io
  */
#ifndef AFL_U_T_IO_HPP
#define AFL_U_T_IO_HPP

#include <cxxtest/TestSuite.h>

class TestIoBufferedSink : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestIoBufferedStream : public CxxTest::TestSuite {
 public:
    void testReadByte();
    void testMixedIO();
};

class TestIoConstMemoryStream : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestIoDataSink : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestIoDirectory : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestIoDirectoryEntry : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestIoFileMapping : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestIoFileSystem : public CxxTest::TestSuite {
 public:
    void testGeneric();
    void testPosix();
    void testWin32();
    void testRoot();
};

class TestIoInflateDataSink : public CxxTest::TestSuite {
 public:
    void testGood();
    void testBad();
};

class TestIoInflateTransform : public CxxTest::TestSuite {
 public:
    void testGood();
    void testGood2();
    void testBad();
};

class TestIoInternalDirectory : public CxxTest::TestSuite {
 public:
    void testBaseAPI();
    void testCreation();
    void testIteration();
    void testConcurrency();
};

class TestIoInternalFileMapping : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestIoInternalSink : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestIoInternalStream : public CxxTest::TestSuite {
 public:
    void testIt();
    void testOverflow();
    void testZero();
    void testName();
};

class TestIoLimitedDataSink : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestIoMemoryStream : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestIoMultiDirectory : public CxxTest::TestSuite {
 public:
    void testCreate();
    void testLookup();
    void testEnum();
    void testEmpty();
};

class TestIoMultiplexableStream : public CxxTest::TestSuite {
 public:
    void testAccess();
    void testLife();
};

class TestIoNullFileSystem : public CxxTest::TestSuite {
 public:
    void testOpen();
    void testArithmetic();
};

class TestIoNullStream : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestIoNullTextWriter : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestIoStream : public CxxTest::TestSuite {
 public:
    void testIt();
    void testInterface();
    void testCopy();
};

class TestIoTextFile : public CxxTest::TestSuite {
 public:
    void testReadLine();
    void testEncoding();
    void testWrite();

 private:
    void doReadLineTest(const char* data, const char* name);
};

class TestIoTextReader : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestIoTextSink : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestIoTextWriter : public CxxTest::TestSuite {
 public:
    void testIt();
    void testNewline();
};

class TestIoTransform : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestIoTransformDataSink : public CxxTest::TestSuite {
 public:
    void testNull();
    void testSimple();
    void testLarge();
};

#endif
