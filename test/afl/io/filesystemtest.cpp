/**
  *  \file test/afl/io/filesystemtest.cpp
  *  \brief Test for afl::io::FileSystem
  */

#include "afl/io/filesystem.hpp"

#include "afl/except/fileproblemexception.hpp"
#include "afl/io/directory.hpp"
#include "afl/io/directoryentry.hpp"
#include "afl/io/internaldirectory.hpp"
#include "afl/io/nullstream.hpp"
#include "afl/io/temporarydirectory.hpp"
#include "afl/test/testrunner.hpp"

using afl::base::Ptr;
using afl::base::Ref;
using afl::except::FileProblemException;
using afl::io::Directory;
using afl::io::DirectoryEntry;
using afl::io::FileSystem;
using afl::io::InternalDirectory;
using afl::io::TemporaryDirectory;

/** Generic tests. */
AFL_TEST("afl.io.FileSystem:path-names:generic", a)
{
    FileSystem& fs = FileSystem::getInstance();

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
    FileSystem& fs = FileSystem::getInstance();

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
    AFL_CHECK_THROWS(a("openDirectory .nul"), fs.openDirectory(String_t(".\0Q", 3))->getDirectoryEntries(), FileProblemException);
    AFL_CHECK_THROWS(a("openDirectory /nul"), fs.openDirectory(String_t("/\0Q", 3))->getDirectoryEntries(), FileProblemException);
}
#endif

#if TARGET_OS_WIN32
/** Win32 tests. */
AFL_TEST("afl.io.FileSystem:path-names:win32", a)
{
    FileSystem& fs = FileSystem::getInstance();

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
    FileSystem& fs = FileSystem::getInstance();

    // Must be able to open it
    Ref<Directory> root = fs.openRootDirectory();

    // Root properties:
    // - no parent, no name
    a.check("01. getParentDirectory", root->getParentDirectory().get() == 0);
    a.checkEqual("02. getDirectoryName", root->getDirectoryName(), "");
    // - but a title
    a.check("03. getTitle", !root->getTitle().empty());
    // - cannot access files
    AFL_CHECK_THROWS(a("04. createAsDirectory"), root->getDirectoryEntryByName("x")->createAsDirectory(), FileProblemException);

    // Enumerate content
    Ref<afl::base::Enumerator<Ptr<DirectoryEntry> > > entries = root->getDirectoryEntries();
    a.check("11. getDirectoryEntries", entries.asPtr().get() != 0);

    Ptr<DirectoryEntry> entry;
    size_t n = 0;
    while (entries->getNextElement(entry)) {
        // Verify the entry
        a.check("21. getNextElement", entry.get() != 0);
        a.check("22. getTitle", !entry->getTitle().empty());
        a.checkEqual("23. openContainingDirectory", &*entry->openContainingDirectory(), &*root);
        a.check("24. getFileType", entry->getFileType() == DirectoryEntry::tDirectory);
        if (entry->getFlags().contains(DirectoryEntry::Link)) {
            a.check("25. getLinkText", !entry->getLinkText().empty());
        }

        // Cannot be modified
        AFL_CHECK_THROWS(a("26. renameTo"), entry->renameTo("foo"), FileProblemException);
        AFL_CHECK_THROWS(a("27. erase"), entry->erase(), FileProblemException);
        AFL_CHECK_THROWS(a("28. createAsDirectory"), entry->createAsDirectory(), FileProblemException);
        AFL_CHECK_THROWS(a("29. setFlag"), entry->setFlag(DirectoryEntry::Hidden, true), FileProblemException);

        ++n;
    }

    // Every system has nonzero real roots. If not, the setup is broken and we cannot with good conscience succeed the tests.
    a.check("31. num roots", n > 0);
}

/** Test openFileNT. Also serves as an interface test. */
AFL_TEST("afl.io.FileSystem:openFileNT", a)
{
    class Tester : public FileSystem {
     public:
        virtual Ref<afl::io::Stream> openFile(FileName_t fileName, OpenMode /*mode*/)
            {
                if (fileName == "fpe") {
                    throw FileProblemException(fileName, "boom");
                } else if (fileName == "ex") {
                    throw std::runtime_error("err");
                } else {
                    return *new afl::io::NullStream();
                }
            }
        virtual Ref<Directory> openDirectory(FileName_t /*dirName*/)
            { throw std::runtime_error("no ref"); }
        virtual Ref<Directory> openRootDirectory()
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

/** Test moving between two directories. */
AFL_TEST("afl.io.FileSystem:moveFile:two-directories", a)
{
    FileSystem& fs = FileSystem::getInstance();
    Ref<Directory> dir = fs.openDirectory(fs.getWorkingDirectoryName());
    std::auto_ptr<TemporaryDirectory> dir1(new TemporaryDirectory(dir));
    std::auto_ptr<TemporaryDirectory> dir2(new TemporaryDirectory(dir));
    Ref<InternalDirectory> internal = InternalDirectory::create("int");

    dir1->get()->openFile("file1", FileSystem::Create)->fullWrite(afl::string::toBytes("a"));
    dir1->get()->openFile("file2", FileSystem::Create)->fullWrite(afl::string::toBytes("bb"));
    dir1->get()->openFile("file3", FileSystem::Create)->fullWrite(afl::string::toBytes("ccc"));

    // Direct rename
    AFL_CHECK_SUCCEEDS(a("01. direct rename succeeds"), dir1->get()->getDirectoryEntryByName("file1")->moveTo(*dir2->get(), "new1"));

    // No-throw rename
    a.checkEqual("02. no-throw rename", dir1->get()->getDirectoryEntryByName("file2")->moveToNT(*dir2->get(), "new2"), true);

    // Failure
    AFL_CHECK_THROWS(a("03. cross-device fails"), dir1->get()->getDirectoryEntryByName("file3")->moveTo(*internal, "new3"), FileProblemException);
    a.checkEqual("04. cross-device fails", dir1->get()->getDirectoryEntryByName("file3")->moveToNT(*internal, "new3"), false);

    // Verify resulting status
    a.checkEqual("81. result", dir2->get()->getDirectoryEntryByName("new1")->getFileSize(), 1U);
    a.checkEqual("82. result", dir2->get()->getDirectoryEntryByName("new2")->getFileSize(), 2U);
    a.checkEqual("83. result", dir1->get()->getDirectoryEntryByName("file3")->getFileSize(), 3U);
}

/** Test moving within one directory. */
AFL_TEST("afl.io.FileSystem:moveFile:one-directory", a)
{
    FileSystem& fs = FileSystem::getInstance();
    Ref<Directory> dir = fs.openDirectory(fs.getWorkingDirectoryName());
    std::auto_ptr<TemporaryDirectory> dir1(new TemporaryDirectory(dir));

    dir1->get()->openFile("file1", FileSystem::Create)->fullWrite(afl::string::toBytes("a"));

    // Direct rename
    AFL_CHECK_SUCCEEDS(a("01. rename succeeds"), dir1->get()->getDirectoryEntryByName("file1")->moveTo(*dir1->get(), "new1"));

    // Verify resulting status
    a.checkEqual("81. result", dir1->get()->getDirectoryEntryByName("new1")->getFileSize(), 1U);
}

/** Test moving: source does not exist. */
AFL_TEST("afl.io.FileSystem:moveFile:error:source-missing", a)
{
    FileSystem& fs = FileSystem::getInstance();
    Ref<Directory> dir = fs.openDirectory(fs.getWorkingDirectoryName());
    std::auto_ptr<TemporaryDirectory> dir1(new TemporaryDirectory(dir));

    // Direct rename
    AFL_CHECK_THROWS(a("01. rename succeeds"), dir1->get()->getDirectoryEntryByName("file1")->moveTo(*dir1->get(), "new1"), FileProblemException);
}

/** Test moving a directory. */
AFL_TEST("afl.io.FileSystem:moveFile:dir", a)
{
    FileSystem& fs = FileSystem::getInstance();
    Ref<Directory> dir = fs.openDirectory(fs.getWorkingDirectoryName());
    std::auto_ptr<TemporaryDirectory> dir1(new TemporaryDirectory(dir));

    dir1->get()->getDirectoryEntryByName("file1")->createAsDirectory();

    // Direct rename
    AFL_CHECK_SUCCEEDS(a("01. rename fails"), dir1->get()->getDirectoryEntryByName("file1")->moveTo(*dir1->get(), "new1"));
    a.checkEqual("02. type", dir1->get()->getDirectoryEntryByName("new1")->getFileType(), DirectoryEntry::tDirectory);
}

/** Test overwriting a file with another one. */
AFL_TEST("afl.io.FileSystem:moveFile:file-overwrites-file", a)
{
    FileSystem& fs = FileSystem::getInstance();
    Ref<Directory> dir = fs.openDirectory(fs.getWorkingDirectoryName());
    std::auto_ptr<TemporaryDirectory> dir1(new TemporaryDirectory(dir));

    dir1->get()->openFile("file1", FileSystem::Create)->fullWrite(afl::string::toBytes("a"));
    dir1->get()->openFile("new1", FileSystem::Create)->fullWrite(afl::string::toBytes("xxx"));

    // Direct rename
    AFL_CHECK_SUCCEEDS(a("01. rename fails"), dir1->get()->getDirectoryEntryByName("file1")->moveTo(*dir1->get(), "new1"));
    a.checkEqual("02. size", dir1->get()->getDirectoryEntryByName("new1")->getFileSize(), 1U);
}

/** Test error: incompatible overwrite. */
AFL_TEST("afl.io.FileSystem:moveFile:error:dir-overwrites-file", a)
{
    FileSystem& fs = FileSystem::getInstance();
    Ref<Directory> dir = fs.openDirectory(fs.getWorkingDirectoryName());
    std::auto_ptr<TemporaryDirectory> dir1(new TemporaryDirectory(dir));

    dir1->get()->getDirectoryEntryByName("file1")->createAsDirectory();
    dir1->get()->openFile("new1", FileSystem::Create);

    // Direct rename
#if TARGET_OS_WIN32
    AFL_CHECK_SUCCEEDS(a("01. rename succeeds on Win32"), dir1->get()->getDirectoryEntryByName("file1")->moveTo(*dir1->get(), "new1"));
#else
    AFL_CHECK_THROWS(a("01. rename fails"), dir1->get()->getDirectoryEntryByName("file1")->moveTo(*dir1->get(), "new1"), FileProblemException);
#endif
}

/** Test error: incompatible overwrite. */
AFL_TEST("afl.io.FileSystem:moveFile:error:file-overwrites-dir", a)
{
    FileSystem& fs = FileSystem::getInstance();
    Ref<Directory> dir = fs.openDirectory(fs.getWorkingDirectoryName());
    std::auto_ptr<TemporaryDirectory> dir1(new TemporaryDirectory(dir));

    dir1->get()->openFile("file1", FileSystem::Create);
    dir1->get()->getDirectoryEntryByName("new1")->createAsDirectory();

    // Direct rename
    AFL_CHECK_THROWS(a("01. rename fails"), dir1->get()->getDirectoryEntryByName("file1")->moveTo(*dir1->get(), "new1"), FileProblemException);
}

/** Test moving between two incompatible directories. */
AFL_TEST("afl.io.FileSystem:moveFile:by-copying", a)
{
    FileSystem& fs = FileSystem::getInstance();
    Ref<Directory> dir = fs.openDirectory(fs.getWorkingDirectoryName());
    std::auto_ptr<TemporaryDirectory> dir1(new TemporaryDirectory(dir));
    Ref<InternalDirectory> internal = InternalDirectory::create("int");

    dir1->get()->openFile("file1", FileSystem::Create)->fullWrite(afl::string::toBytes("a"));

    // Direct move fails, by-copy succeeds
    a.checkEqual("01. cross-device fails", dir1->get()->getDirectoryEntryByName("file1")->moveToNT(*internal, "new1"), false);
    AFL_CHECK_SUCCEEDS(a("02. move succeeds"), dir1->get()->getDirectoryEntryByName("file1")->moveFileByCopying(*internal, "new1"));

    // Verify resulting status
    a.checkEqual("81. result", internal->getDirectoryEntryByName("new1")->getFileSize(), 1U);
}

/** Test moving by copy, error: source is a directory */
AFL_TEST("afl.io.FileSystem:moveFile:by-copying:error:source-is-dir", a)
{
    FileSystem& fs = FileSystem::getInstance();
    Ref<Directory> dir = fs.openDirectory(fs.getWorkingDirectoryName());
    std::auto_ptr<TemporaryDirectory> dir1(new TemporaryDirectory(dir));
    Ref<InternalDirectory> internal = InternalDirectory::create("int");

    dir1->get()->getDirectoryEntryByName("file1")->createAsDirectory();

    AFL_CHECK_THROWS(a("01. move fails"), dir1->get()->getDirectoryEntryByName("file1")->moveFileByCopying(*internal, "new1"), FileProblemException);
}

/** Test moving by copy, error: target is a directory */
AFL_TEST("afl.io.FileSystem:moveFile:by-copying:error:target-is-dir", a)
{
    FileSystem& fs = FileSystem::getInstance();
    Ref<Directory> dir = fs.openDirectory(fs.getWorkingDirectoryName());
    std::auto_ptr<TemporaryDirectory> dir1(new TemporaryDirectory(dir));
    Ref<InternalDirectory> internal = InternalDirectory::create("int");

    internal->openFile("file1", FileSystem::Create)->fullWrite(afl::string::toBytes("a"));
    dir1->get()->getDirectoryEntryByName("new1")->createAsDirectory();

    AFL_CHECK_THROWS(a("01. move fails"), internal->getDirectoryEntryByName("file1")->moveFileByCopying(*dir1->get(), "new1"), FileProblemException);
}
