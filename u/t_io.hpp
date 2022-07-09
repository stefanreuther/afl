/**
  *  \file u/t_io.hpp
  *  \brief Test for afl::io
  */
#ifndef AFL_U_T_IO_HPP
#define AFL_U_T_IO_HPP

#include <cxxtest/TestSuite.h>
#include "afl/io/stream.hpp"

class TestIoBufferedSink : public CxxTest::TestSuite {
 public:
    void testIt();
    void testFail();
    void testFail2();
    void testFail3();
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

class TestIoDeflateTransform : public CxxTest::TestSuite {
 public:
    void testGzip();
    void testRaw();
    void testZlib();
};

class TestIoDirectory : public CxxTest::TestSuite {
 public:
    void testDirectoryAccess2();
    void testParent();
    void testEraseContentRecursively();
    void testEraseContentRecursivelyLive();
    void setUp();
    void tearDown();

    void testInterface();
    void testIt();
    void testCreationAndDeletion();
    void testDirectoryAccess();
};

class TestIoDirectoryEntry : public CxxTest::TestSuite {
 public:
    void testIt();
    void testInfo();
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
    void testOpenFileNT();
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
    void testTruncate();
    void testVariant();
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
    void testMem();
};

class TestIoInternalFileSystem : public CxxTest::TestSuite {
 public:
    void testFileNames();
    void testRoot();
    void testContent();
    void testEnum();
    void testEntry();
    void testErase();
    void testRename();
    void testCreate();
    void testCreateDirectory();
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
    void testReadOnly();
};

class TestIoInternalTextWriter : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestIoLimitedDataSink : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestIoLimitedStream : public CxxTest::TestSuite {
 public:
    void testFull();
};

class TestIoMemoryStream : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestIoMsExpandTransform : public CxxTest::TestSuite {
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
    void testSeek();
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

class TestIoTemporaryDirectory : public CxxTest::TestSuite {
 public:
    void testLive();
    void testInternal();
    void testFail();
    void testFailErase();
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

class TestIoTransformReaderStream : public CxxTest::TestSuite {
 public:
    void testIt();
    void testExtra();
    void testPosition();
};

class TestIoUnchangeableDirectoryEntry : public CxxTest::TestSuite {
 public:
    void testIt();
};

#endif
