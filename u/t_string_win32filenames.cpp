/**
  *  \file u/t_string_win32filenames.cpp
  *  \brief Test for afl::string::Win32FileNames
  */

#include "afl/string/win32filenames.hpp"

#include "t_string.hpp"

/** Test flipSlashes(). */
void
TestStringWin32FileNames::testFlipSlashes()
{
    afl::string::Win32FileNames p;
    String_t s = "a/b";
    p.flipSlashes(s);
    TS_ASSERT_EQUALS(s, "a\\b");

    s = "/x/y/z/";
    p.flipSlashes(s);
    TS_ASSERT_EQUALS(s, "\\x\\y\\z\\");

    s = "\\o/";
    p.flipSlashes(s);
    TS_ASSERT_EQUALS(s, "\\o\\");

    s = "//x";
    p.flipSlashes(s);
    TS_ASSERT_EQUALS(s, "\\\\x");
}

/** Test isAbsolutePathName(). */
void
TestStringWin32FileNames::testIsAbsolute()
{
    afl::string::Win32FileNames p;

    TS_ASSERT(!p.isAbsolutePathName("/foo"));
    TS_ASSERT(!p.isAbsolutePathName("\\foo"));
    TS_ASSERT(!p.isAbsolutePathName("a:foo"));
    TS_ASSERT(p.isAbsolutePathName("a:/foo"));
    TS_ASSERT(p.isAbsolutePathName("a:\\foo"));
    TS_ASSERT(!p.isAbsolutePathName("a:b/foo"));
    TS_ASSERT(!p.isAbsolutePathName("a:b\\foo"));
    TS_ASSERT(!p.isAbsolutePathName("a"));
    TS_ASSERT(!p.isAbsolutePathName("a/"));
    TS_ASSERT(!p.isAbsolutePathName("a///"));
    TS_ASSERT(!p.isAbsolutePathName("."));
}

/** Test isPathSeparator(). */
void
TestStringWin32FileNames::testIsPathSeparator()
{
    afl::string::Win32FileNames p;

    TS_ASSERT(p.isPathSeparator('/'));
    TS_ASSERT(p.isPathSeparator('\\'));
    TS_ASSERT(!p.isPathSeparator('\0'));
    TS_ASSERT(p.isPathSeparator(':'));
}

/** Test makePathName(). */
void
TestStringWin32FileNames::testMakePath()
{
    afl::string::Win32FileNames p;

    TS_ASSERT_EQUALS(p.makePathName("a", "b"), "a\\b");
    TS_ASSERT_EQUALS(p.makePathName("a/", "b"), "a/b");
    TS_ASSERT_EQUALS(p.makePathName("a\\", "b"), "a\\b");
    TS_ASSERT_EQUALS(p.makePathName("a:", "b"), "a:b");
    TS_ASSERT_EQUALS(p.makePathName("", "b"), "b");

    TS_ASSERT_EQUALS(p.makePathName("a", ""), "a\\");
    TS_ASSERT_EQUALS(p.makePathName("a", "/"), "/");
    TS_ASSERT_EQUALS(p.makePathName("a", "\\"), "\\");
    TS_ASSERT_EQUALS(p.makePathName("a/", "/foo"), "/foo");
}

/** Test getCanonicalPathName(). */
void
TestStringWin32FileNames::testGetCanonicalPathName()
{
    afl::string::Win32FileNames p;

    TS_ASSERT_EQUALS(p.getCanonicalPathName("a/b/c"), "a\\b\\c");
    TS_ASSERT_EQUALS(p.getCanonicalPathName("a///c"), "a\\c");
    TS_ASSERT_EQUALS(p.getCanonicalPathName("a/./c"), "a\\c");
    TS_ASSERT_EQUALS(p.getCanonicalPathName("a/../c"), "c");

    TS_ASSERT_EQUALS(p.getCanonicalPathName("a\\.\\c"), "a\\c");

    TS_ASSERT_EQUALS(p.getCanonicalPathName("/a/b/c"), "\\a\\b\\c");
    TS_ASSERT_EQUALS(p.getCanonicalPathName("/a///c"), "\\a\\c");
    TS_ASSERT_EQUALS(p.getCanonicalPathName("/a/./c"), "\\a\\c");
    TS_ASSERT_EQUALS(p.getCanonicalPathName("/a/../c"), "\\c");

    TS_ASSERT_EQUALS(p.getCanonicalPathName("../a/b/c"), "..\\a\\b\\c");
    TS_ASSERT_EQUALS(p.getCanonicalPathName("..\\a\\b\\c"), "..\\a\\b\\c");

    TS_ASSERT_EQUALS(p.getCanonicalPathName(""), ".");
    TS_ASSERT_EQUALS(p.getCanonicalPathName("."), ".");
    TS_ASSERT_EQUALS(p.getCanonicalPathName("/"), "\\");
    TS_ASSERT_EQUALS(p.getCanonicalPathName("/."), "\\");
    TS_ASSERT_EQUALS(p.getCanonicalPathName("\\"), "\\");
    TS_ASSERT_EQUALS(p.getCanonicalPathName("\\."), "\\");
    TS_ASSERT_EQUALS(p.getCanonicalPathName("c:"), "c:.");
    TS_ASSERT_EQUALS(p.getCanonicalPathName("c:\\"), "c:\\");
    TS_ASSERT_EQUALS(p.getCanonicalPathName("c:\\.\\foo"), "c:\\foo");
    TS_ASSERT_EQUALS(p.getCanonicalPathName("c:\\..\\foo"), "c:\\foo");
}

/** Test getFileName(). */
void
TestStringWin32FileNames::testGetFileName()
{
    afl::string::Win32FileNames p;

    TS_ASSERT_EQUALS(p.getFileName(""), "");
    TS_ASSERT_EQUALS(p.getFileName("aa"), "aa");
    TS_ASSERT_EQUALS(p.getFileName("/aa"), "aa");
    TS_ASSERT_EQUALS(p.getFileName("\\aa"), "aa");
    TS_ASSERT_EQUALS(p.getFileName("c:foo"), "foo");
    TS_ASSERT_EQUALS(p.getFileName("c:\\foo"), "foo");
    TS_ASSERT_EQUALS(p.getFileName("a/b/c"), "c");
    TS_ASSERT_EQUALS(p.getFileName("a/b/.."), "..");
    TS_ASSERT_EQUALS(p.getFileName("a\\b/c"), "c");
    TS_ASSERT_EQUALS(p.getFileName("a/b\\.."), "..");
}

/** Test getDirectoryName(). */
void
TestStringWin32FileNames::testGetDirectoryName()
{
    afl::string::Win32FileNames p;

    TS_ASSERT_EQUALS(p.getDirectoryName(""), ".");
    TS_ASSERT_EQUALS(p.getDirectoryName("aa"), ".");
    TS_ASSERT_EQUALS(p.getDirectoryName("/aa"), "\\");
    TS_ASSERT_EQUALS(p.getDirectoryName("\\aa"), "\\");
    TS_ASSERT_EQUALS(p.getDirectoryName("c:foo"), "c:");
    TS_ASSERT_EQUALS(p.getDirectoryName("c:\\foo"), "c:\\");
    TS_ASSERT_EQUALS(p.getDirectoryName("a\\b/c"), "a\\b");
    TS_ASSERT_EQUALS(p.getDirectoryName("a/b\\.."), "a\\b");
}

