/**
  *  \file u/t_io_nullfilesystem.cpp
  *  \brief Test for afl::io::NullFileSystem
  */

#include "afl/io/nullfilesystem.hpp"

#include "u/t_io.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/io/directory.hpp"

/** Test "open" functions.
    They must all fail. */
void
TestIoNullFileSystem::testOpen()
{
    afl::io::NullFileSystem fs;
    TS_ASSERT_THROWS(fs.openFile("name", fs.Create), afl::except::FileProblemException);
    TS_ASSERT_THROWS(fs.openFile("name", fs.OpenRead), afl::except::FileProblemException);
    TS_ASSERT_THROWS(fs.openDirectory("name"), afl::except::FileProblemException);
    TS_ASSERT_THROWS(fs.openRootDirectory(), afl::except::FileProblemException);
}

/** Test file name arithmetic functions.
    Just a quick litmus test; they are tested in PosixFileNames. */
void
TestIoNullFileSystem::testArithmetic()
{
    afl::io::NullFileSystem fs;

    // Just a quick check
    // isPathSeparator
    TS_ASSERT(fs.isPathSeparator('/'));
    TS_ASSERT(!fs.isPathSeparator('\\'));

    // isAbsolutePathName
    TS_ASSERT(fs.isAbsolutePathName("/foo"));
    TS_ASSERT(!fs.isAbsolutePathName("a"));
    TS_ASSERT(!fs.isAbsolutePathName("."));

    // makePathName
    TS_ASSERT_EQUALS(fs.makePathName("a", "b"), "a/b");
    TS_ASSERT_EQUALS(fs.makePathName("a", "/"), "/");
    TS_ASSERT_EQUALS(fs.makePathName("a/", "/foo"), "/foo");

    // getCanonicalPathName
    TS_ASSERT_EQUALS(fs.getCanonicalPathName("a/b/c"), "a/b/c");
    TS_ASSERT_EQUALS(fs.getCanonicalPathName("a///c"), "a/c");
    TS_ASSERT_EQUALS(fs.getCanonicalPathName("/a/../c"), "/c");
    TS_ASSERT_EQUALS(fs.getCanonicalPathName("../a/b/c"), "../a/b/c");
    TS_ASSERT_EQUALS(fs.getCanonicalPathName(""), ".");

    // getAbsolutePathName -- see generic test
    TS_ASSERT_EQUALS(fs.getAbsolutePathName("a"), "/a");
    TS_ASSERT_EQUALS(fs.getAbsolutePathName("../b"), "/b");

    // getFileName
    TS_ASSERT_EQUALS(fs.getFileName(""), "");
    TS_ASSERT_EQUALS(fs.getFileName("aa"), "aa");
    TS_ASSERT_EQUALS(fs.getFileName("a/b/c"), "c");

    // getDirectoryName
    TS_ASSERT_EQUALS(fs.getDirectoryName(""), ".");
    TS_ASSERT_EQUALS(fs.getDirectoryName("aa"), ".");
    TS_ASSERT_EQUALS(fs.getDirectoryName("/aa"), "/");
}
