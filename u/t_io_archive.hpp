/**
  *  \file u/t_io_archive.hpp
  *  \brief Tests for afl::io::archive
  */
#ifndef AFL_U_T_IO_ARCHIVE_HPP
#define AFL_U_T_IO_ARCHIVE_HPP

#include <cxxtest/TestSuite.h>

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
