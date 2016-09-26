/**
  *  \file u/t_io_filesystem.cpp
  *  \brief Test for afl::io::FileSystem
  */

#include "afl/io/filesystem.hpp"

#include "u/t_io.hpp"
#include "afl/io/directory.hpp"
#include "afl/io/directoryentry.hpp"

/** Generic tests. */
void
TestIoFileSystem::testGeneric()
{
    afl::io::FileSystem& fs = afl::io::FileSystem::getInstance();

    // Empty string never is absolute
    TS_ASSERT(!fs.isAbsolutePathName(""));

    // Working directory never is empty
    TS_ASSERT(fs.getWorkingDirectoryName() != "");

    // Working directory is absolute
    TS_ASSERT(fs.isAbsolutePathName(fs.getWorkingDirectoryName()));

    // getAbsolutePathName returns an absolute path name
    TS_ASSERT(fs.isAbsolutePathName(fs.getAbsolutePathName("x")));
}

/** POSIX tests. */
void
TestIoFileSystem::testPosix()
{
#if TARGET_OS_POSIX
    afl::io::FileSystem& fs = afl::io::FileSystem::getInstance();

    // isPathSeparator
    TS_ASSERT(fs.isPathSeparator('/'));
    TS_ASSERT(!fs.isPathSeparator('\\'));
    TS_ASSERT(!fs.isPathSeparator('\0'));
    TS_ASSERT(!fs.isPathSeparator(':'));

    // isAbsolutePathName
    TS_ASSERT(fs.isAbsolutePathName("/foo"));
    TS_ASSERT(fs.isAbsolutePathName("//foo"));
    TS_ASSERT(fs.isAbsolutePathName("///foo/bar"));
    TS_ASSERT(!fs.isAbsolutePathName("a"));
    TS_ASSERT(!fs.isAbsolutePathName("a/"));
    TS_ASSERT(!fs.isAbsolutePathName("a///"));
    TS_ASSERT(!fs.isAbsolutePathName("."));

    // makePathName
    TS_ASSERT_EQUALS(fs.makePathName("a", "b"), "a/b");
    TS_ASSERT_EQUALS(fs.makePathName("a/", "b"), "a/b");
    TS_ASSERT_EQUALS(fs.makePathName("", "b"), "b");
    TS_ASSERT_EQUALS(fs.makePathName("a", ""), "a/");

    TS_ASSERT_EQUALS(fs.makePathName("a", "/"), "/");
    TS_ASSERT_EQUALS(fs.makePathName("a/", "/foo"), "/foo");

    // getCanonicalPathName
    TS_ASSERT_EQUALS(fs.getCanonicalPathName("a/b/c"), "a/b/c");
    TS_ASSERT_EQUALS(fs.getCanonicalPathName("a///c"), "a/c");
    TS_ASSERT_EQUALS(fs.getCanonicalPathName("a/./c"), "a/c");
    TS_ASSERT_EQUALS(fs.getCanonicalPathName("a/../c"), "c");

    TS_ASSERT_EQUALS(fs.getCanonicalPathName("/a/b/c"), "/a/b/c");
    TS_ASSERT_EQUALS(fs.getCanonicalPathName("/a///c"), "/a/c");
    TS_ASSERT_EQUALS(fs.getCanonicalPathName("/a/./c"), "/a/c");
    TS_ASSERT_EQUALS(fs.getCanonicalPathName("/a/../c"), "/c");

    TS_ASSERT_EQUALS(fs.getCanonicalPathName("../a/b/c"), "../a/b/c");

    TS_ASSERT_EQUALS(fs.getCanonicalPathName(""), ".");
    TS_ASSERT_EQUALS(fs.getCanonicalPathName("."), ".");
    TS_ASSERT_EQUALS(fs.getCanonicalPathName("/"), "/");
    TS_ASSERT_EQUALS(fs.getCanonicalPathName("/."), "/");

    // getAbsolutePathName -- see generic test

    // getFileName
    TS_ASSERT_EQUALS(fs.getFileName(""), "");
    TS_ASSERT_EQUALS(fs.getFileName("aa"), "aa");
    TS_ASSERT_EQUALS(fs.getFileName("/aa"), "aa");
    TS_ASSERT_EQUALS(fs.getFileName("a/b/c"), "c");
    TS_ASSERT_EQUALS(fs.getFileName("a/b/.."), "..");

    // getDirectoryName
    TS_ASSERT_EQUALS(fs.getDirectoryName(""), ".");
    TS_ASSERT_EQUALS(fs.getDirectoryName("aa"), ".");
    TS_ASSERT_EQUALS(fs.getDirectoryName("/aa"), "/");
    TS_ASSERT_EQUALS(fs.getDirectoryName("a/b/c"), "a/b");
    TS_ASSERT_EQUALS(fs.getDirectoryName("a/b/.."), "a/b");

    // getWorkingDirectoryName -- see generic test
#endif
}

/** Win32 tests. */
void
TestIoFileSystem::testWin32()
{
#if TARGET_OS_WIN32
    afl::io::FileSystem& fs = afl::io::FileSystem::getInstance();

    // isPathSeparator
    TS_ASSERT(fs.isPathSeparator('/'));
    TS_ASSERT(fs.isPathSeparator('\\'));
    TS_ASSERT(!fs.isPathSeparator('\0'));
    TS_ASSERT(fs.isPathSeparator(':'));

    // isAbsolutePathName
    TS_ASSERT(!fs.isAbsolutePathName("/foo"));
    TS_ASSERT(!fs.isAbsolutePathName("\\foo"));
    TS_ASSERT(!fs.isAbsolutePathName("a:foo"));
    TS_ASSERT(fs.isAbsolutePathName("a:/foo"));
    TS_ASSERT(fs.isAbsolutePathName("a:\\foo"));
    TS_ASSERT(!fs.isAbsolutePathName("a:b/foo"));
    TS_ASSERT(!fs.isAbsolutePathName("a:b\\foo"));
    TS_ASSERT(!fs.isAbsolutePathName("a"));
    TS_ASSERT(!fs.isAbsolutePathName("a/"));
    TS_ASSERT(!fs.isAbsolutePathName("a///"));
    TS_ASSERT(!fs.isAbsolutePathName("."));

    // makePathName
    TS_ASSERT_EQUALS(fs.makePathName("a", "b"), "a\\b");
    TS_ASSERT_EQUALS(fs.makePathName("a/", "b"), "a/b");
    TS_ASSERT_EQUALS(fs.makePathName("a\\", "b"), "a\\b");
    TS_ASSERT_EQUALS(fs.makePathName("a:", "b"), "a:b");
    TS_ASSERT_EQUALS(fs.makePathName("", "b"), "b");

    TS_ASSERT_EQUALS(fs.makePathName("a", ""), "a\\");
    TS_ASSERT_EQUALS(fs.makePathName("a", "/"), "/");
    TS_ASSERT_EQUALS(fs.makePathName("a", "\\"), "\\");
    TS_ASSERT_EQUALS(fs.makePathName("a/", "/foo"), "/foo");

    // getCanonicalPathName
    TS_ASSERT_EQUALS(fs.getCanonicalPathName("a/b/c"), "a\\b\\c");
    TS_ASSERT_EQUALS(fs.getCanonicalPathName("a///c"), "a\\c");
    TS_ASSERT_EQUALS(fs.getCanonicalPathName("a/./c"), "a\\c");
    TS_ASSERT_EQUALS(fs.getCanonicalPathName("a/../c"), "c");

    TS_ASSERT_EQUALS(fs.getCanonicalPathName("a\\.\\c"), "a\\c");

    TS_ASSERT_EQUALS(fs.getCanonicalPathName("/a/b/c"), "\\a\\b\\c");
    TS_ASSERT_EQUALS(fs.getCanonicalPathName("/a///c"), "\\a\\c");
    TS_ASSERT_EQUALS(fs.getCanonicalPathName("/a/./c"), "\\a\\c");
    TS_ASSERT_EQUALS(fs.getCanonicalPathName("/a/../c"), "\\c");

    TS_ASSERT_EQUALS(fs.getCanonicalPathName("../a/b/c"), "..\\a\\b\\c");
    TS_ASSERT_EQUALS(fs.getCanonicalPathName("..\\a\\b\\c"), "..\\a\\b\\c");

    TS_ASSERT_EQUALS(fs.getCanonicalPathName(""), ".");
    TS_ASSERT_EQUALS(fs.getCanonicalPathName("."), ".");
    TS_ASSERT_EQUALS(fs.getCanonicalPathName("/"), "\\");
    TS_ASSERT_EQUALS(fs.getCanonicalPathName("/."), "\\");
    TS_ASSERT_EQUALS(fs.getCanonicalPathName("\\"), "\\");
    TS_ASSERT_EQUALS(fs.getCanonicalPathName("\\."), "\\");
    TS_ASSERT_EQUALS(fs.getCanonicalPathName("c:"), "c:.");
    TS_ASSERT_EQUALS(fs.getCanonicalPathName("c:\\"), "c:\\");
    TS_ASSERT_EQUALS(fs.getCanonicalPathName("c:\\.\\foo"), "c:\\foo");
    TS_ASSERT_EQUALS(fs.getCanonicalPathName("c:\\..\\foo"), "c:\\foo");

    // getAbsolutePathName -- see generic test

    // getFileName
    TS_ASSERT_EQUALS(fs.getFileName(""), "");
    TS_ASSERT_EQUALS(fs.getFileName("aa"), "aa");
    TS_ASSERT_EQUALS(fs.getFileName("/aa"), "aa");
    TS_ASSERT_EQUALS(fs.getFileName("\\aa"), "aa");
    TS_ASSERT_EQUALS(fs.getFileName("c:foo"), "foo");
    TS_ASSERT_EQUALS(fs.getFileName("c:\\foo"), "foo");
    TS_ASSERT_EQUALS(fs.getFileName("a/b/c"), "c");
    TS_ASSERT_EQUALS(fs.getFileName("a/b/.."), "..");
    TS_ASSERT_EQUALS(fs.getFileName("a\\b/c"), "c");
    TS_ASSERT_EQUALS(fs.getFileName("a/b\\.."), "..");

    // getDirectoryName
    TS_ASSERT_EQUALS(fs.getDirectoryName(""), ".");
    TS_ASSERT_EQUALS(fs.getDirectoryName("aa"), ".");
    TS_ASSERT_EQUALS(fs.getDirectoryName("/aa"), "\\");
    TS_ASSERT_EQUALS(fs.getDirectoryName("\\aa"), "\\");
    TS_ASSERT_EQUALS(fs.getDirectoryName("c:foo"), "c:");
    TS_ASSERT_EQUALS(fs.getDirectoryName("c:\\foo"), "c:\\");
    TS_ASSERT_EQUALS(fs.getDirectoryName("a\\b/c"), "a\\b");
    TS_ASSERT_EQUALS(fs.getDirectoryName("a/b\\.."), "a\\b");

    // getWorkingDirectoryName -- see generic test
#endif
}

/** Test iteration of roots.
    The system instance must allow successful traversal of the root. */
void
TestIoFileSystem::testRoot()
{
    afl::io::FileSystem& fs = afl::io::FileSystem::getInstance();

    // Must be able to open it
    afl::base::Ptr<afl::io::Directory> root = fs.openRootDirectory();
    TS_ASSERT(root.get() != 0);

    // Root has no parent
    TS_ASSERT(root->getParentDirectory().get() == 0);

    // Enumerate content
    afl::base::Ptr<afl::base::Enumerator<afl::base::Ptr<afl::io::DirectoryEntry> > > entries = root->getDirectoryEntries();
    TS_ASSERT(entries.get() != 0);

    afl::base::Ptr<afl::io::DirectoryEntry> entry;
    size_t n = 0;
    while (entries->getNextElement(entry)) {
        TS_ASSERT(entry.get() != 0);
        ++n;
    }

    // Every system has nonzero real roots. If not, the setup is broken and we cannot with good conscience succeed the tests.
    TS_ASSERT(n > 0);
}
