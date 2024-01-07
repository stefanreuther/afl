/**
  *  \file test/afl/io/filesystemtest.cpp
  *  \brief Test for afl::io::FileSystem
  */

#include "afl/io/filesystem.hpp"

#include "afl/except/fileproblemexception.hpp"
#include "afl/io/directory.hpp"
#include "afl/io/directoryentry.hpp"
#include "afl/io/nullstream.hpp"
#include "afl/test/testrunner.hpp"

/** Generic tests. */
AFL_TEST("afl.io.FileSystem:path-names:generic", a)
{
    afl::io::FileSystem& fs = afl::io::FileSystem::getInstance();

    a.check("empty string never is absolute",
            !fs.isAbsolutePathName(""));

    a.check("working directory is not empty",
            fs.getWorkingDirectoryName() != "");

    a.check("working directory is absolute",
            fs.isAbsolutePathName(fs.getWorkingDirectoryName()));

    a.check("getAbsolutePathName returns absolute path name", fs.isAbsolutePathName(fs.getAbsolutePathName("x")));
}

#if TARGET_OS_POSIX
/** POSIX tests. */
AFL_TEST("afl.io.FileSystem:path-names:posix", a)
{
    afl::io::FileSystem& fs = afl::io::FileSystem::getInstance();

    // isPathSeparator
    a.check("isPathSeparator 1", fs.isPathSeparator('/'));
    a.check("isPathSeparator 2", !fs.isPathSeparator('\\'));
    a.check("isPathSeparator 3", !fs.isPathSeparator('\0'));
    a.check("isPathSeparator 4", !fs.isPathSeparator(':'));

    // isAbsolutePathName
    a.check("isAbsolutePathName 1", fs.isAbsolutePathName("/foo"));
    a.check("isAbsolutePathName 2", fs.isAbsolutePathName("//foo"));
    a.check("isAbsolutePathName 3", fs.isAbsolutePathName("///foo/bar"));
    a.check("isAbsolutePathName 4", !fs.isAbsolutePathName("a"));
    a.check("isAbsolutePathName 5", !fs.isAbsolutePathName("a/"));
    a.check("isAbsolutePathName 6", !fs.isAbsolutePathName("a///"));
    a.check("isAbsolutePathName 7", !fs.isAbsolutePathName("."));

    // makePathName
    a.checkEqual("makePathName 1", fs.makePathName("a", "b"), "a/b");
    a.checkEqual("makePathName 2", fs.makePathName("a/", "b"), "a/b");
    a.checkEqual("makePathName 3", fs.makePathName("", "b"), "b");
    a.checkEqual("makePathName 4", fs.makePathName("a", ""), "a/");

    a.checkEqual("makePathName 5", fs.makePathName("a", "/"), "/");
    a.checkEqual("makePathName 6", fs.makePathName("a/", "/foo"), "/foo");

    // getCanonicalPathName
    a.checkEqual("getCanonicalPathName 1", fs.getCanonicalPathName("a/b/c"), "a/b/c");
    a.checkEqual("getCanonicalPathName 2", fs.getCanonicalPathName("a///c"), "a/c");
    a.checkEqual("getCanonicalPathName 3", fs.getCanonicalPathName("a/./c"), "a/c");
    a.checkEqual("getCanonicalPathName 4", fs.getCanonicalPathName("a/../c"), "c");

    a.checkEqual("getCanonicalPathName 5", fs.getCanonicalPathName("/a/b/c"), "/a/b/c");
    a.checkEqual("getCanonicalPathName 6", fs.getCanonicalPathName("/a///c"), "/a/c");
    a.checkEqual("getCanonicalPathName 7", fs.getCanonicalPathName("/a/./c"), "/a/c");
    a.checkEqual("getCanonicalPathName 8", fs.getCanonicalPathName("/a/../c"), "/c");

    a.checkEqual("getCanonicalPathName 9", fs.getCanonicalPathName("../a/b/c"), "../a/b/c");

    a.checkEqual("getCanonicalPathName A", fs.getCanonicalPathName(""), ".");
    a.checkEqual("getCanonicalPathName B", fs.getCanonicalPathName("."), ".");
    a.checkEqual("getCanonicalPathName C", fs.getCanonicalPathName("/"), "/");
    a.checkEqual("getCanonicalPathName D", fs.getCanonicalPathName("/."), "/");

    // getAbsolutePathName -- see generic test

    // getFileName
    a.checkEqual("getFileName 1", fs.getFileName(""), "");
    a.checkEqual("getFileName 2", fs.getFileName("aa"), "aa");
    a.checkEqual("getFileName 3", fs.getFileName("/aa"), "aa");
    a.checkEqual("getFileName 4", fs.getFileName("a/b/c"), "c");
    a.checkEqual("getFileName 5", fs.getFileName("a/b/.."), "..");

    // getDirectoryName
    a.checkEqual("getDirectoryName 1", fs.getDirectoryName(""), ".");
    a.checkEqual("getDirectoryName 2", fs.getDirectoryName("aa"), ".");
    a.checkEqual("getDirectoryName 3", fs.getDirectoryName("/aa"), "/");
    a.checkEqual("getDirectoryName 4", fs.getDirectoryName("a/b/c"), "a/b");
    a.checkEqual("getDirectoryName 5", fs.getDirectoryName("a/b/.."), "a/b");

    // getWorkingDirectoryName -- see generic test

    // Reading a directory: opening "." or "/" works, but check that null is correctly rejected
    AFL_CHECK_SUCCEEDS(a("openDirectory ."), fs.openDirectory(".")->getDirectoryEntries());
    AFL_CHECK_SUCCEEDS(a("openDirectory /"), fs.openDirectory("/")->getDirectoryEntries());
    AFL_CHECK_THROWS(a("openDirectory .nul"), fs.openDirectory(String_t(".\0Q", 3))->getDirectoryEntries(), afl::except::FileProblemException);
    AFL_CHECK_THROWS(a("openDirectory /nul"), fs.openDirectory(String_t("/\0Q", 3))->getDirectoryEntries(), afl::except::FileProblemException);
}
#endif

#if TARGET_OS_WIN32
/** Win32 tests. */
AFL_TEST("afl.io.FileSystem:path-names:win32", a)
{
    afl::io::FileSystem& fs = afl::io::FileSystem::getInstance();

    // isPathSeparator
    a.check("isPathSeparator 1", fs.isPathSeparator('/'));
    a.check("isPathSeparator 2", fs.isPathSeparator('\\'));
    a.check("isPathSeparator 3", !fs.isPathSeparator('\0'));
    a.check("isPathSeparator 4", fs.isPathSeparator(':'));

    // isAbsolutePathName
    a.check("isAbsolutePathName 1", !fs.isAbsolutePathName("/foo"));
    a.check("isAbsolutePathName 2", !fs.isAbsolutePathName("\\foo"));
    a.check("isAbsolutePathName 3", !fs.isAbsolutePathName("a:foo"));
    a.check("isAbsolutePathName 4", fs.isAbsolutePathName("a:/foo"));
    a.check("isAbsolutePathName 5", fs.isAbsolutePathName("a:\\foo"));
    a.check("isAbsolutePathName 6", !fs.isAbsolutePathName("a:b/foo"));
    a.check("isAbsolutePathName 7", !fs.isAbsolutePathName("a:b\\foo"));
    a.check("isAbsolutePathName 8", !fs.isAbsolutePathName("a"));
    a.check("isAbsolutePathName 9", !fs.isAbsolutePathName("a/"));
    a.check("isAbsolutePathName A", !fs.isAbsolutePathName("a///"));
    a.check("isAbsolutePathName B", !fs.isAbsolutePathName("."));

    // makePathName
    a.checkEqual("makePathName 1", fs.makePathName("a", "b"), "a\\b");
    a.checkEqual("makePathName 2", fs.makePathName("a/", "b"), "a/b");
    a.checkEqual("makePathName 3", fs.makePathName("a\\", "b"), "a\\b");
    a.checkEqual("makePathName 4", fs.makePathName("a:", "b"), "a:b");
    a.checkEqual("makePathName 5", fs.makePathName("", "b"), "b");

    a.checkEqual("makePathName 6", fs.makePathName("a", ""), "a\\");
    a.checkEqual("makePathName 7", fs.makePathName("a", "/"), "/");
    a.checkEqual("makePathName 8", fs.makePathName("a", "\\"), "\\");
    a.checkEqual("makePathName 9", fs.makePathName("a/", "/foo"), "/foo");

    // getCanonicalPathName
    a.checkEqual("getCanonicalPathName 01", fs.getCanonicalPathName("a/b/c"), "a\\b\\c");
    a.checkEqual("getCanonicalPathName 02", fs.getCanonicalPathName("a///c"), "a\\c");
    a.checkEqual("getCanonicalPathName 03", fs.getCanonicalPathName("a/./c"), "a\\c");
    a.checkEqual("getCanonicalPathName 04", fs.getCanonicalPathName("a/../c"), "c");

    a.checkEqual("getCanonicalPathName 05", fs.getCanonicalPathName("a\\.\\c"), "a\\c");

    a.checkEqual("getCanonicalPathName 06", fs.getCanonicalPathName("/a/b/c"), "\\a\\b\\c");
    a.checkEqual("getCanonicalPathName 07", fs.getCanonicalPathName("/a///c"), "\\a\\c");
    a.checkEqual("getCanonicalPathName 08", fs.getCanonicalPathName("/a/./c"), "\\a\\c");
    a.checkEqual("getCanonicalPathName 09", fs.getCanonicalPathName("/a/../c"), "\\c");

    a.checkEqual("getCanonicalPathName 10", fs.getCanonicalPathName("../a/b/c"), "..\\a\\b\\c");
    a.checkEqual("getCanonicalPathName 11", fs.getCanonicalPathName("..\\a\\b\\c"), "..\\a\\b\\c");

    a.checkEqual("getCanonicalPathName 12", fs.getCanonicalPathName(""), ".");
    a.checkEqual("getCanonicalPathName 13", fs.getCanonicalPathName("."), ".");
    a.checkEqual("getCanonicalPathName 14", fs.getCanonicalPathName("/"), "\\");
    a.checkEqual("getCanonicalPathName 15", fs.getCanonicalPathName("/."), "\\");
    a.checkEqual("getCanonicalPathName 16", fs.getCanonicalPathName("\\"), "\\");
    a.checkEqual("getCanonicalPathName 17", fs.getCanonicalPathName("\\."), "\\");
    a.checkEqual("getCanonicalPathName 18", fs.getCanonicalPathName("c:"), "c:.");
    a.checkEqual("getCanonicalPathName 19", fs.getCanonicalPathName("c:\\"), "c:\\");
    a.checkEqual("getCanonicalPathName 20", fs.getCanonicalPathName("c:\\.\\foo"), "c:\\foo");
    a.checkEqual("getCanonicalPathName 21", fs.getCanonicalPathName("c:\\..\\foo"), "c:\\foo");

    // getAbsolutePathName -- see generic test

    // getFileName
    a.checkEqual("getFileName 1", fs.getFileName(""), "");
    a.checkEqual("getFileName 2", fs.getFileName("aa"), "aa");
    a.checkEqual("getFileName 3", fs.getFileName("/aa"), "aa");
    a.checkEqual("getFileName 4", fs.getFileName("\\aa"), "aa");
    a.checkEqual("getFileName 5", fs.getFileName("c:foo"), "foo");
    a.checkEqual("getFileName 6", fs.getFileName("c:\\foo"), "foo");
    a.checkEqual("getFileName 7", fs.getFileName("a/b/c"), "c");
    a.checkEqual("getFileName 8", fs.getFileName("a/b/.."), "..");
    a.checkEqual("getFileName 9", fs.getFileName("a\\b/c"), "c");
    a.checkEqual("getFileName A", fs.getFileName("a/b\\.."), "..");

    // getDirectoryName
    a.checkEqual("getDirectoryName 1", fs.getDirectoryName(""), ".");
    a.checkEqual("getDirectoryName 2", fs.getDirectoryName("aa"), ".");
    a.checkEqual("getDirectoryName 3", fs.getDirectoryName("/aa"), "\\");
    a.checkEqual("getDirectoryName 4", fs.getDirectoryName("\\aa"), "\\");
    a.checkEqual("getDirectoryName 5", fs.getDirectoryName("c:foo"), "c:");
    a.checkEqual("getDirectoryName 6", fs.getDirectoryName("c:\\foo"), "c:\\");
    a.checkEqual("getDirectoryName 7", fs.getDirectoryName("a\\b/c"), "a\\b");
    a.checkEqual("getDirectoryName 8", fs.getDirectoryName("a/b\\.."), "a\\b");

    // getWorkingDirectoryName -- see generic test
}
#endif

/** Test iteration of roots.
    The system instance must allow successful traversal of the root. */
AFL_TEST("afl.io.FileSystem:openRootDirectory", a)
{
    afl::io::FileSystem& fs = afl::io::FileSystem::getInstance();

    // Must be able to open it
    afl::base::Ref<afl::io::Directory> root = fs.openRootDirectory();

    // Root properties:
    // - no parent, no name
    a.check("01. getParentDirectory", root->getParentDirectory().get() == 0);
    a.checkEqual("02. getDirectoryName", root->getDirectoryName(), "");
    // - but a title
    a.check("03. getTitle", !root->getTitle().empty());
    // - cannot access files
    AFL_CHECK_THROWS(a("04. createAsDirectory"), root->getDirectoryEntryByName("x")->createAsDirectory(), afl::except::FileProblemException);

    // Enumerate content
    afl::base::Ref<afl::base::Enumerator<afl::base::Ptr<afl::io::DirectoryEntry> > > entries = root->getDirectoryEntries();
    a.check("11. getDirectoryEntries", &entries.get() != 0);

    afl::base::Ptr<afl::io::DirectoryEntry> entry;
    size_t n = 0;
    while (entries->getNextElement(entry)) {
        // Verify the entry
        a.check("21. getNextElement", entry.get() != 0);
        a.check("22. getTitle", !entry->getTitle().empty());
        a.checkEqual("23. openContainingDirectory", &*entry->openContainingDirectory(), &*root);
        a.check("24. getFileType", entry->getFileType() == afl::io::DirectoryEntry::tDirectory);
        if (entry->getFlags().contains(afl::io::DirectoryEntry::Link)) {
            a.check("25. getLinkText", !entry->getLinkText().empty());
        }

        // Cannot be modified
        AFL_CHECK_THROWS(a("26. renameTo"), entry->renameTo("foo"), afl::except::FileProblemException);
        AFL_CHECK_THROWS(a("27. erase"), entry->erase(), afl::except::FileProblemException);
        AFL_CHECK_THROWS(a("28. createAsDirectory"), entry->createAsDirectory(), afl::except::FileProblemException);
        AFL_CHECK_THROWS(a("29. setFlag"), entry->setFlag(afl::io::DirectoryEntry::Hidden, true), afl::except::FileProblemException);

        ++n;
    }

    // Every system has nonzero real roots. If not, the setup is broken and we cannot with good conscience succeed the tests.
    a.check("31. num roots", n > 0);
}

/** Test openFileNT. Also serves as an interface test. */
AFL_TEST("afl.io.FileSystem:openFileNT", a)
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
    a.check("fpe", t.openFileNT("fpe", Tester::OpenRead).get() == 0);
    a.check("other", t.openFileNT("other", Tester::OpenRead).get() != 0);
    AFL_CHECK_THROWS(a("ex"), t.openFileNT("ex", Tester::OpenRead), std::runtime_error);
}
