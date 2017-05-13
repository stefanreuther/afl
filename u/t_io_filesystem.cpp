/**
  *  \file u/t_io_filesystem.cpp
  *  \brief Test for afl::io::FileSystem
  */

#include "afl/io/filesystem.hpp"

#include "u/t_io.hpp"
#include "afl/io/directory.hpp"
#include "afl/io/directoryentry.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/io/nullstream.hpp"

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

    // Reading a directory: opening "." or "/" works, but check that null is correctly rejected
    TS_ASSERT_THROWS_NOTHING(fs.openDirectory(".")->getDirectoryEntries());
    TS_ASSERT_THROWS_NOTHING(fs.openDirectory("/")->getDirectoryEntries());
    TS_ASSERT_THROWS(fs.openDirectory(String_t(".\0Q", 3))->getDirectoryEntries(), afl::except::FileProblemException);
    TS_ASSERT_THROWS(fs.openDirectory(String_t("/\0Q", 3))->getDirectoryEntries(), afl::except::FileProblemException);
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
    afl::base::Ref<afl::io::Directory> root = fs.openRootDirectory();

    // Root properties:
    // - no parent, no name
    TS_ASSERT(root->getParentDirectory().get() == 0);
    TS_ASSERT_EQUALS(root->getDirectoryName(), "");
    // - but a title
    TS_ASSERT(!root->getTitle().empty());
    // - cannot access files
    TS_ASSERT_THROWS(root->getDirectoryEntryByName("x")->createAsDirectory(), afl::except::FileProblemException);

    // Enumerate content
    afl::base::Ref<afl::base::Enumerator<afl::base::Ptr<afl::io::DirectoryEntry> > > entries = root->getDirectoryEntries();
    TS_ASSERT(&entries.get() != 0);

    afl::base::Ptr<afl::io::DirectoryEntry> entry;
    size_t n = 0;
    while (entries->getNextElement(entry)) {
        // Verify the entry
        TS_ASSERT(entry.get() != 0);
        TS_ASSERT(!entry->getTitle().empty());
        TS_ASSERT_EQUALS(&*entry->openContainingDirectory(), &*root);
        TS_ASSERT_EQUALS(entry->getFileType(), afl::io::DirectoryEntry::tDirectory);
        if (entry->getFlags().contains(afl::io::DirectoryEntry::Link)) {
            TS_ASSERT(!entry->getLinkText().empty());
        }

        // Cannot be modified
        TS_ASSERT_THROWS(entry->renameTo("foo"), afl::except::FileProblemException);
        TS_ASSERT_THROWS(entry->erase(), afl::except::FileProblemException);
        TS_ASSERT_THROWS(entry->createAsDirectory(), afl::except::FileProblemException);
        TS_ASSERT_THROWS(entry->setFlag(afl::io::DirectoryEntry::Hidden, true), afl::except::FileProblemException);

        ++n;
    }

    // Every system has nonzero real roots. If not, the setup is broken and we cannot with good conscience succeed the tests.
    TS_ASSERT(n > 0);
}

/** Test openFileNT. Also serves as an interface test. */
void
TestIoFileSystem::testOpenFileNT()
{
    class Tester : public afl::io::FileSystem {
     public:
        virtual afl::base::Ref<afl::io::Stream> openFile(FileName_t fileName, OpenMode /*mode*/)
            {
                if (fileName == "fpe") {
                    throw afl::except::FileProblemException(fileName, "boom");
                } else if (fileName == "ex") {
                    throw std::runtime_error("err");
                } else {
                    return *new afl::io::NullStream();
                }
            }
        virtual afl::base::Ref<afl::io::Directory> openDirectory(FileName_t /*dirName*/)
            { throw std::runtime_error("no ref"); }
        virtual afl::base::Ref<afl::io::Directory> openRootDirectory()
            { throw std::runtime_error("no ref"); }
        virtual bool isAbsolutePathName(FileName_t /*path*/)
            { return false; }
        virtual bool isPathSeparator(char /*c*/)
            { return false; }
        virtual FileSystem::FileName_t makePathName(FileName_t /*path*/, FileName_t /*name*/)
            { return FileName_t(); }
        virtual FileSystem::FileName_t getCanonicalPathName(FileName_t /*name*/)
            { return FileName_t(); }
        virtual FileSystem::FileName_t getAbsolutePathName(FileName_t /*name*/)
            { return FileName_t(); }
        virtual FileSystem::FileName_t getFileName(FileName_t /*name*/)
            { return FileName_t(); }
        virtual FileSystem::FileName_t getDirectoryName(FileName_t /*name*/)
            { return FileName_t(); }
        virtual FileSystem::FileName_t getWorkingDirectoryName()
            { return FileName_t(); }
    };
    Tester t;

    // Test openFileNT
    TS_ASSERT(t.openFileNT("fpe", Tester::OpenRead).get() == 0);
    TS_ASSERT(t.openFileNT("other", Tester::OpenRead).get() != 0);
    TS_ASSERT_THROWS(t.openFileNT("ex", Tester::OpenRead), std::runtime_error);
}
