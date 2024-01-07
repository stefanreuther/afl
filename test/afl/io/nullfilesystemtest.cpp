/**
  *  \file test/afl/io/nullfilesystemtest.cpp
  *  \brief Test for afl::io::NullFileSystem
  */

#include "afl/io/nullfilesystem.hpp"

#include "afl/except/fileproblemexception.hpp"
#include "afl/io/directory.hpp"
#include "afl/test/testrunner.hpp"

/** Test "open" functions.
    They must all fail. */
AFL_TEST("afl.io.NullFileSystem:open-functions", a)
{
    afl::io::NullFileSystem fs;
    AFL_CHECK_THROWS(a("openFile to create fails"), fs.openFile("name", fs.Create), afl::except::FileProblemException);
    AFL_CHECK_THROWS(a("openFile to read fails"),   fs.openFile("name", fs.OpenRead), afl::except::FileProblemException);
    AFL_CHECK_THROWS(a("openDirectory fails"),      fs.openDirectory("name"), afl::except::FileProblemException);
    AFL_CHECK_THROWS(a("openRootDirectory fails"),  fs.openRootDirectory(), afl::except::FileProblemException);
}

/** Test file name arithmetic functions.
    Just a quick litmus test; they are tested in PosixFileNames. */
AFL_TEST("afl.io.NullFileSystem:filename-functions", a)
{
    afl::io::NullFileSystem fs;

    // Just a quick check
    // isPathSeparator
    a.check("isPathSeparator 1", fs.isPathSeparator('/'));
    a.check("isPathSeparator 2", !fs.isPathSeparator('\\'));

    // isAbsolutePathName
    a.check("isAbsolutePathName 1", fs.isAbsolutePathName("/foo"));
    a.check("isAbsolutePathName 2", !fs.isAbsolutePathName("a"));
    a.check("isAbsolutePathName 3", !fs.isAbsolutePathName("."));

    // makePathName
    a.checkEqual("makePathName 1", fs.makePathName("a", "b"), "a/b");
    a.checkEqual("makePathName 2", fs.makePathName("a", "/"), "/");
    a.checkEqual("makePathName 3", fs.makePathName("a/", "/foo"), "/foo");

    // getCanonicalPathName
    a.checkEqual("getCanonicalPathName 1", fs.getCanonicalPathName("a/b/c"), "a/b/c");
    a.checkEqual("getCanonicalPathName 2", fs.getCanonicalPathName("a///c"), "a/c");
    a.checkEqual("getCanonicalPathName 3", fs.getCanonicalPathName("/a/../c"), "/c");
    a.checkEqual("getCanonicalPathName 4", fs.getCanonicalPathName("../a/b/c"), "../a/b/c");
    a.checkEqual("getCanonicalPathName 5", fs.getCanonicalPathName(""), ".");

    // getAbsolutePathName -- see generic test
    a.checkEqual("getAbsolutePathName 1", fs.getAbsolutePathName("a"), "/a");
    a.checkEqual("getAbsolutePathName 2", fs.getAbsolutePathName("../b"), "/b");

    // getFileName
    a.checkEqual("getFileName 1", fs.getFileName(""), "");
    a.checkEqual("getFileName 2", fs.getFileName("aa"), "aa");
    a.checkEqual("getFileName 3", fs.getFileName("a/b/c"), "c");

    // getDirectoryName
    a.checkEqual("getDirectoryName 1", fs.getDirectoryName(""), ".");
    a.checkEqual("getDirectoryName 2", fs.getDirectoryName("aa"), ".");
    a.checkEqual("getDirectoryName 3", fs.getDirectoryName("/aa"), "/");
}
