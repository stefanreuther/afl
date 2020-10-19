/**
  *  \file u/t_io_archive.hpp
  *  \brief Tests for afl::io::archive
  */
#ifndef AFL_U_T_IO_ARCHIVE_HPP
#define AFL_U_T_IO_ARCHIVE_HPP

#include <cxxtest/TestSuite.h>

class TestIoArchiveArReader : public CxxTest::TestSuite {
 public:
    void testList();
    void testList2();
    void testList3();
    void testRead();
    void testFailure();
};

class TestIoArchiveTarReader : public CxxTest::TestSuite {
 public:
    void testListCompressed();
    void testListWithPath();
    void testListUncompressed();
    void testReadCompressed();
    void testReadUncompressed();
    void testNameCompressed();
};

class TestIoArchiveZipReader : public CxxTest::TestSuite {
 public:
    void testReadMembers();
    void testBadOperations();
    void testMeta();
    void testUncompr();
    void testCrypt();
    void testUnsupported();
};

#endif
