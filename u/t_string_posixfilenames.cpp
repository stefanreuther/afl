/**
  *  \file u/t_string_posixfilenames.cpp
  *  \brief Test for afl::string::PosixFileNames
  */

#include "afl/string/posixfilenames.hpp"

#include "u/t_string.hpp"

/** Test PosixFileNames::isAbsolutePathName. */
void
TestStringPosixFileNames::testIsAbsolute()
{
    afl::string::PosixFileNames p;

    TS_ASSERT(!p.isAbsolutePathName(""));
    TS_ASSERT(!p.isAbsolutePathName("."));
    TS_ASSERT(!p.isAbsolutePathName("~"));
    TS_ASSERT(!p.isAbsolutePathName("foo"));
    TS_ASSERT(!p.isAbsolutePathName("a///"));

    TS_ASSERT(p.isAbsolutePathName("/"));
    TS_ASSERT(p.isAbsolutePathName("/tmp"));
    TS_ASSERT(p.isAbsolutePathName("/.."));
    TS_ASSERT(p.isAbsolutePathName("/foo"));
    TS_ASSERT(p.isAbsolutePathName("//foo"));
    TS_ASSERT(p.isAbsolutePathName("///foo/bar"));
}

/** Test PosixFileNames::isPathSeparator. */
void
TestStringPosixFileNames::testIsPathSep()
{
    afl::string::PosixFileNames p;

    TS_ASSERT(p.isPathSeparator('/'));
    TS_ASSERT(!p.isPathSeparator('~'));
    TS_ASSERT(!p.isPathSeparator(';'));
    TS_ASSERT(!p.isPathSeparator('\\'));
    TS_ASSERT(!p.isPathSeparator(0));
}

/** Test PosixFileNames::makePathName. */
void
TestStringPosixFileNames::testMakePath()
{
    afl::string::PosixFileNames p;

    TS_ASSERT_EQUALS(p.makePathName("a", "b"), "a/b");
    TS_ASSERT_EQUALS(p.makePathName("a/", "b"), "a/b");
    TS_ASSERT_EQUALS(p.makePathName("", "b"), "b");
    TS_ASSERT_EQUALS(p.makePathName("a", ""), "a/");

    TS_ASSERT_EQUALS(p.makePathName("a", "/"), "/");
    TS_ASSERT_EQUALS(p.makePathName("a/", "/foo"), "/foo");

    TS_ASSERT_EQUALS(p.makePathName("/", "../foo"), "/../foo");
}

/** Test PosixFileNames::getCanonicalPathName. */
void
TestStringPosixFileNames::testGetCanonical()
{
    afl::string::PosixFileNames p;

    TS_ASSERT_EQUALS(p.getCanonicalPathName("a/b/c"), "a/b/c");
    TS_ASSERT_EQUALS(p.getCanonicalPathName("a///c"), "a/c");
    TS_ASSERT_EQUALS(p.getCanonicalPathName("a/./c"), "a/c");
    TS_ASSERT_EQUALS(p.getCanonicalPathName("a/../c"), "c");

    TS_ASSERT_EQUALS(p.getCanonicalPathName("/a/b/c"), "/a/b/c");
    TS_ASSERT_EQUALS(p.getCanonicalPathName("/a///c"), "/a/c");
    TS_ASSERT_EQUALS(p.getCanonicalPathName("/a/./c"), "/a/c");
    TS_ASSERT_EQUALS(p.getCanonicalPathName("/a/../c"), "/c");

    TS_ASSERT_EQUALS(p.getCanonicalPathName("../a/b/c"), "../a/b/c");
    TS_ASSERT_EQUALS(p.getCanonicalPathName("../a/../../b/c"), "../../b/c");
    TS_ASSERT_EQUALS(p.getCanonicalPathName("../../a/../b/c"), "../../b/c");

    TS_ASSERT_EQUALS(p.getCanonicalPathName(""), ".");
    TS_ASSERT_EQUALS(p.getCanonicalPathName("."), ".");
    TS_ASSERT_EQUALS(p.getCanonicalPathName("/"), "/");
    TS_ASSERT_EQUALS(p.getCanonicalPathName("/."), "/");

    TS_ASSERT_EQUALS(p.getCanonicalPathName("/../foo"), "/foo");
}

/** Test PosixFileNames::getFileName. */
void
TestStringPosixFileNames::testGetFileName()
{
    afl::string::PosixFileNames p;

    TS_ASSERT_EQUALS(p.getFileName(""), "");
    TS_ASSERT_EQUALS(p.getFileName("aa"), "aa");
    TS_ASSERT_EQUALS(p.getFileName("/aa"), "aa");
    TS_ASSERT_EQUALS(p.getFileName("a/b/c"), "c");
    TS_ASSERT_EQUALS(p.getFileName("a/b/.."), "..");
    TS_ASSERT_EQUALS(p.getFileName("a/b/."), ".");
}

/** Test PosixFileNames::getDirectoryName. */
void
TestStringPosixFileNames::testGetDirName()
{
    afl::string::PosixFileNames p;

    TS_ASSERT_EQUALS(p.getDirectoryName(""), ".");
    TS_ASSERT_EQUALS(p.getDirectoryName("aa"), ".");
    TS_ASSERT_EQUALS(p.getDirectoryName("/aa"), "/");
    TS_ASSERT_EQUALS(p.getDirectoryName("a/b/c"), "a/b");
    TS_ASSERT_EQUALS(p.getDirectoryName("a/b/.."), "a/b");
}
