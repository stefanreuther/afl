/**
  *  \file test/afl/string/posixfilenamestest.cpp
  *  \brief Test for afl::string::PosixFileNames
  */

#include "afl/string/posixfilenames.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.string.PosixFileNames:isAbsolutePathName", a)
{
    afl::string::PosixFileNames p;

    a.check("01", !p.isAbsolutePathName(""));
    a.check("02", !p.isAbsolutePathName("."));
    a.check("03", !p.isAbsolutePathName("~"));
    a.check("04", !p.isAbsolutePathName("foo"));
    a.check("05", !p.isAbsolutePathName("a///"));

    a.check("11", p.isAbsolutePathName("/"));
    a.check("12", p.isAbsolutePathName("/tmp"));
    a.check("13", p.isAbsolutePathName("/.."));
    a.check("14", p.isAbsolutePathName("/foo"));
    a.check("15", p.isAbsolutePathName("//foo"));
    a.check("16", p.isAbsolutePathName("///foo/bar"));
}

AFL_TEST("afl.string.PosixFileNames:isPathSeparator", a)
{
    afl::string::PosixFileNames p;

    a.check("01", p.isPathSeparator('/'));
    a.check("02", !p.isPathSeparator('~'));
    a.check("03", !p.isPathSeparator(';'));
    a.check("04", !p.isPathSeparator('\\'));
    a.check("05", !p.isPathSeparator(0));
}

AFL_TEST("afl.string.PosixFileNames:makePathName", a)
{
    afl::string::PosixFileNames p;

    a.checkEqual("01", p.makePathName("a", "b"), "a/b");
    a.checkEqual("02", p.makePathName("a/", "b"), "a/b");
    a.checkEqual("03", p.makePathName("", "b"), "b");
    a.checkEqual("04", p.makePathName("a", ""), "a/");

    a.checkEqual("11", p.makePathName("a", "/"), "/");
    a.checkEqual("12", p.makePathName("a/", "/foo"), "/foo");

    a.checkEqual("21", p.makePathName("/", "../foo"), "/../foo");

    a.checkEqual("31", p.makePathName(".", "x.txt"), "x.txt");
}

AFL_TEST("afl.string.PosixFileNames:getCanonicalPathName", a)
{
    afl::string::PosixFileNames p;

    a.checkEqual("01", p.getCanonicalPathName("a/b/c"), "a/b/c");
    a.checkEqual("02", p.getCanonicalPathName("a///c"), "a/c");
    a.checkEqual("03", p.getCanonicalPathName("a/./c"), "a/c");
    a.checkEqual("04", p.getCanonicalPathName("a/../c"), "c");

    a.checkEqual("11", p.getCanonicalPathName("/a/b/c"), "/a/b/c");
    a.checkEqual("12", p.getCanonicalPathName("/a///c"), "/a/c");
    a.checkEqual("13", p.getCanonicalPathName("/a/./c"), "/a/c");
    a.checkEqual("14", p.getCanonicalPathName("/a/../c"), "/c");

    a.checkEqual("21", p.getCanonicalPathName("../a/b/c"), "../a/b/c");
    a.checkEqual("22", p.getCanonicalPathName("../a/../../b/c"), "../../b/c");
    a.checkEqual("23", p.getCanonicalPathName("../../a/../b/c"), "../../b/c");

    a.checkEqual("31", p.getCanonicalPathName(""), ".");
    a.checkEqual("32", p.getCanonicalPathName("."), ".");
    a.checkEqual("33", p.getCanonicalPathName("/"), "/");
    a.checkEqual("34", p.getCanonicalPathName("/."), "/");

    a.checkEqual("41", p.getCanonicalPathName("/../foo"), "/foo");
}

AFL_TEST("afl.string.PosixFileNames:getFileName", a)
{
    afl::string::PosixFileNames p;

    a.checkEqual("01", p.getFileName(""), "");
    a.checkEqual("02", p.getFileName("aa"), "aa");
    a.checkEqual("03", p.getFileName("/aa"), "aa");
    a.checkEqual("04", p.getFileName("a/b/c"), "c");
    a.checkEqual("05", p.getFileName("a/b/.."), "..");
    a.checkEqual("06", p.getFileName("a/b/."), ".");
}

AFL_TEST("afl.string.PosixFileNames:getDirectoryName", a)
{
    afl::string::PosixFileNames p;

    a.checkEqual("01", p.getDirectoryName(""), ".");
    a.checkEqual("02", p.getDirectoryName("aa"), ".");
    a.checkEqual("03", p.getDirectoryName("/aa"), "/");
    a.checkEqual("04", p.getDirectoryName("a/b/c"), "a/b");
    a.checkEqual("05", p.getDirectoryName("a/b/.."), "a/b");
}
