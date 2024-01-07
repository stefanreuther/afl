/**
  *  \file test/afl/string/win32filenamestest.cpp
  *  \brief Test for afl::string::Win32FileNames
  */

#include "afl/string/win32filenames.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.string.Win32FileNames:flipSlashes", a)
{
    afl::string::Win32FileNames p;
    String_t s = "a/b";
    p.flipSlashes(s);
    a.checkEqual("01", s, "a\\b");

    s = "/x/y/z/";
    p.flipSlashes(s);
    a.checkEqual("02", s, "\\x\\y\\z\\");

    s = "\\o/";
    p.flipSlashes(s);
    a.checkEqual("03", s, "\\o\\");

    s = "//x";
    p.flipSlashes(s);
    a.checkEqual("04", s, "\\\\x");
}

AFL_TEST("afl.string.Win32FileNames:isAbsolutePathName", a)
{
    afl::string::Win32FileNames p;

    a.check("01", !p.isAbsolutePathName("/foo"));
    a.check("02", !p.isAbsolutePathName("\\foo"));
    a.check("03", !p.isAbsolutePathName("a:foo"));
    a.check("04", p.isAbsolutePathName("a:/foo"));
    a.check("05", p.isAbsolutePathName("a:\\foo"));
    a.check("06", !p.isAbsolutePathName("a:b/foo"));
    a.check("07", !p.isAbsolutePathName("a:b\\foo"));
    a.check("08", !p.isAbsolutePathName("a"));
    a.check("09", !p.isAbsolutePathName("a/"));
    a.check("10", !p.isAbsolutePathName("a///"));
    a.check("11", !p.isAbsolutePathName("."));
}

AFL_TEST("afl.string.Win32FileNames:isPathSeparator", a)
{
    afl::string::Win32FileNames p;

    a.check("01", p.isPathSeparator('/'));
    a.check("02", p.isPathSeparator('\\'));
    a.check("03", !p.isPathSeparator('\0'));
    a.check("04", p.isPathSeparator(':'));
}

AFL_TEST("afl.string.Win32FileNames:makePathName", a)
{
    afl::string::Win32FileNames p;

    a.checkEqual("01", p.makePathName("a", "b"), "a\\b");
    a.checkEqual("02", p.makePathName("a/", "b"), "a/b");
    a.checkEqual("03", p.makePathName("a\\", "b"), "a\\b");
    a.checkEqual("04", p.makePathName("a:", "b"), "a:b");
    a.checkEqual("05", p.makePathName("", "b"), "b");

    a.checkEqual("11", p.makePathName("a", ""), "a\\");
    a.checkEqual("12", p.makePathName("a", "/"), "/");
    a.checkEqual("13", p.makePathName("a", "\\"), "\\");
    a.checkEqual("14", p.makePathName("a/", "/foo"), "/foo");

    a.checkEqual("21", p.makePathName(".", "f.txt"), "f.txt");
}

AFL_TEST("afl.string.Win32FileNames:getCanonicalPathName", a)
{
    afl::string::Win32FileNames p;

    a.checkEqual("01", p.getCanonicalPathName("a/b/c"), "a\\b\\c");
    a.checkEqual("02", p.getCanonicalPathName("a///c"), "a\\c");
    a.checkEqual("03", p.getCanonicalPathName("a/./c"), "a\\c");
    a.checkEqual("04", p.getCanonicalPathName("a/../c"), "c");

    a.checkEqual("11", p.getCanonicalPathName("a\\.\\c"), "a\\c");

    a.checkEqual("21", p.getCanonicalPathName("/a/b/c"), "\\a\\b\\c");
    a.checkEqual("22", p.getCanonicalPathName("/a///c"), "\\a\\c");
    a.checkEqual("23", p.getCanonicalPathName("/a/./c"), "\\a\\c");
    a.checkEqual("24", p.getCanonicalPathName("/a/../c"), "\\c");

    a.checkEqual("31", p.getCanonicalPathName("../a/b/c"), "..\\a\\b\\c");
    a.checkEqual("32", p.getCanonicalPathName("..\\a\\b\\c"), "..\\a\\b\\c");

    a.checkEqual("41", p.getCanonicalPathName(""), ".");
    a.checkEqual("42", p.getCanonicalPathName("."), ".");
    a.checkEqual("43", p.getCanonicalPathName("/"), "\\");
    a.checkEqual("44", p.getCanonicalPathName("/."), "\\");
    a.checkEqual("45", p.getCanonicalPathName("\\"), "\\");
    a.checkEqual("46", p.getCanonicalPathName("\\."), "\\");
    a.checkEqual("47", p.getCanonicalPathName("c:"), "c:.");
    a.checkEqual("48", p.getCanonicalPathName("c:\\"), "c:\\");
    a.checkEqual("49", p.getCanonicalPathName("c:\\.\\foo"), "c:\\foo");
    a.checkEqual("50", p.getCanonicalPathName("c:\\..\\foo"), "c:\\foo");
}

AFL_TEST("afl.string.Win32FileNames:getFileName", a)
{
    afl::string::Win32FileNames p;

    a.checkEqual("01", p.getFileName(""), "");
    a.checkEqual("02", p.getFileName("aa"), "aa");
    a.checkEqual("03", p.getFileName("/aa"), "aa");
    a.checkEqual("04", p.getFileName("\\aa"), "aa");
    a.checkEqual("05", p.getFileName("c:foo"), "foo");
    a.checkEqual("06", p.getFileName("c:\\foo"), "foo");
    a.checkEqual("07", p.getFileName("a/b/c"), "c");
    a.checkEqual("08", p.getFileName("a/b/.."), "..");
    a.checkEqual("09", p.getFileName("a\\b/c"), "c");
    a.checkEqual("10", p.getFileName("a/b\\.."), "..");
}

AFL_TEST("afl.string.Win32FileNames:getDirectoryName", a)
{
    afl::string::Win32FileNames p;

    a.checkEqual("01", p.getDirectoryName(""), ".");
    a.checkEqual("02", p.getDirectoryName("aa"), ".");
    a.checkEqual("03", p.getDirectoryName("/aa"), "\\");
    a.checkEqual("04", p.getDirectoryName("\\aa"), "\\");
    a.checkEqual("05", p.getDirectoryName("c:foo"), "c:");
    a.checkEqual("06", p.getDirectoryName("c:\\foo"), "c:\\");
    a.checkEqual("07", p.getDirectoryName("a\\b/c"), "a\\b");
    a.checkEqual("08", p.getDirectoryName("a/b\\.."), "a\\b");
}
