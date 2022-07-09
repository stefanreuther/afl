/**
  *  \file u/t_io_internalfilesystem.cpp
  *  \brief Test for afl::io::InternalFileSystem
  */

#include "afl/io/internalfilesystem.hpp"

#include "t_io.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/io/directoryentry.hpp"

using afl::base::Ptr;
using afl::base::Ref;
using afl::except::FileProblemException;
using afl::io::Directory;
using afl::io::DirectoryEntry;
using afl::io::FileSystem;

namespace {
    void prepare(FileSystem& fs)
    {
        fs.openDirectory("/")->getDirectoryEntryByName("d")->createAsDirectory();
        fs.openFile("/d/f1", FileSystem::Create)->fullWrite(afl::string::toBytes("one"));
        fs.openFile("/d/f2", FileSystem::Create)->fullWrite(afl::string::toBytes("two"));
    }
}

/** Test file names.
    Modelled after TestIoFileSystem::testGeneric(), TestIoFileSystem::testPosix(). */
void
TestIoInternalFileSystem::testFileNames()
{
    afl::io::InternalFileSystem fs;

    // getWorkingDirectoryName
    TS_ASSERT(fs.getWorkingDirectoryName() != "");
    TS_ASSERT(fs.isAbsolutePathName(fs.getWorkingDirectoryName()));

    // isPathSeparator
    TS_ASSERT(fs.isPathSeparator('/'));
    TS_ASSERT(!fs.isPathSeparator('\\'));
    TS_ASSERT(!fs.isPathSeparator('\0'));
    TS_ASSERT(!fs.isPathSeparator(':'));

    // isAbsolutePathName
    TS_ASSERT(!fs.isAbsolutePathName(""));
    TS_ASSERT(fs.isAbsolutePathName(fs.getAbsolutePathName("x")));
    TS_ASSERT(fs.isAbsolutePathName("/foo"));
    TS_ASSERT(fs.isAbsolutePathName("//foo"));          // syntactically absolute, but invalid for accessing files
    TS_ASSERT(fs.isAbsolutePathName("///foo/bar"));     // syntactically absolute, but invalid for accessing files
    TS_ASSERT(!fs.isAbsolutePathName("a"));
    TS_ASSERT(!fs.isAbsolutePathName("a/"));
    TS_ASSERT(!fs.isAbsolutePathName("a///"));          // syntactically absolute, but invalid for accessing files
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

    // Reading a directory: opening "." or "/" works, but check that null is correctly rejected
    TS_ASSERT_THROWS_NOTHING(fs.openDirectory(".")->getDirectoryEntries());
    TS_ASSERT_THROWS_NOTHING(fs.openDirectory("/")->getDirectoryEntries());
    TS_ASSERT_THROWS(fs.openDirectory(String_t(".\0Q", 3))->getDirectoryEntries(), FileProblemException);
    TS_ASSERT_THROWS(fs.openDirectory(String_t("/\0Q", 3))->getDirectoryEntries(), FileProblemException);
}

/** Test iteration of roots.
    Modelled after TestIoFileSystem::testRoot() */
void
TestIoInternalFileSystem::testRoot()
{
    afl::io::InternalFileSystem fs;

    // Must be able to open it
    Ref<Directory> root = fs.openRootDirectory();

    // Root properties:
    // - no parent, no name
    TS_ASSERT(root->getParentDirectory().get() == 0);
    TS_ASSERT_EQUALS(root->getDirectoryName(), "");
    // - but a title
    TS_ASSERT(!root->getTitle().empty());
    // - cannot access files
    TS_ASSERT_THROWS(root->getDirectoryEntryByName("x")->createAsDirectory(), FileProblemException);

    // Enumerate content: must be one
    Ref<afl::base::Enumerator<Ptr<DirectoryEntry> > > entries = root->getDirectoryEntries();
    Ptr<DirectoryEntry> entry;
    TS_ASSERT(entries->getNextElement(entry));

    // Verify the entry
    TS_ASSERT(entry.get() != 0);
    TS_ASSERT(!entry->getTitle().empty());
    TS_ASSERT_EQUALS(&*entry->openContainingDirectory(), &*root);
    TS_ASSERT_EQUALS(entry->getFileType(), DirectoryEntry::tDirectory);
    TS_ASSERT(entry->getFlags().contains(DirectoryEntry::Link));
    TS_ASSERT_EQUALS(entry->getLinkText(), "/");

    // Cannot be modified
    TS_ASSERT_THROWS(entry->renameTo("foo"), FileProblemException);
    TS_ASSERT_THROWS(entry->erase(), FileProblemException);
    TS_ASSERT_THROWS(entry->createAsDirectory(), FileProblemException);
    TS_ASSERT_THROWS(entry->setFlag(DirectoryEntry::Hidden, true), FileProblemException);

    // Can open the content as directory
    TS_ASSERT_EQUALS(entry->openDirectory()->getDirectoryName(), "/");
    TS_ASSERT(entry->openDirectory()->getParentDirectory().get() == 0);

    // Cannot open the content as file
    TS_ASSERT_THROWS(entry->openFile(FileSystem::OpenRead), FileProblemException);

    // No more entries
    TS_ASSERT(!entries->getNextElement(entry));
}

/** Test handling of content. */
void
TestIoInternalFileSystem::testContent()
{
    afl::io::InternalFileSystem fs;
    prepare(fs);

    // Error: createAsDirectory when directory exists
    TS_ASSERT_THROWS(fs.openDirectory("/")->getDirectoryEntryByName("d")->createAsDirectory(), FileProblemException);

    // Error: createAsDirectory when file exists
    TS_ASSERT_THROWS(fs.openDirectory("/d")->getDirectoryEntryByName("f1")->createAsDirectory(), FileProblemException);

    // Error: openFile(Create) when directory exists
    TS_ASSERT_THROWS(fs.openDirectory("/")->getDirectoryEntryByName("d")->openFile(FileSystem::Create), FileProblemException);
    TS_ASSERT_THROWS(fs.openFile("/d", FileSystem::Create), FileProblemException);

    // Error: openFile(CreateNew) when file exists
    TS_ASSERT_THROWS(fs.openDirectory("/d")->getDirectoryEntryByName("f1")->openFile(FileSystem::CreateNew), FileProblemException);
    TS_ASSERT_THROWS(fs.openFile("/d/f1", FileSystem::CreateNew), FileProblemException);

    // Error: openFile(OpenRead) when directory exists
    TS_ASSERT_THROWS(fs.openDirectory("/")->getDirectoryEntryByName("d")->openFile(FileSystem::OpenRead), FileProblemException);
    TS_ASSERT_THROWS(fs.openFile("/d", FileSystem::OpenRead), FileProblemException);

    // Error: openFile when container does not exist
    TS_ASSERT_THROWS(fs.openFile("/q/a", FileSystem::OpenRead), FileProblemException);
    TS_ASSERT_THROWS(fs.openFile("/q/a", FileSystem::Create), FileProblemException);

    // Error: openFile when file does not exist
    TS_ASSERT_THROWS(fs.openFile("/d/a", FileSystem::OpenRead), FileProblemException);

    // Error: file name syntax error
    TS_ASSERT_THROWS(fs.openFile("/d//f1", FileSystem::OpenRead), FileProblemException);
    TS_ASSERT_THROWS(fs.openFile("d//f1", FileSystem::OpenRead), FileProblemException);
    TS_ASSERT_THROWS(fs.openFile("d/f1/", FileSystem::OpenRead), FileProblemException);
    TS_ASSERT_THROWS(fs.openDirectory("/")->getDirectoryEntryByName("d/f1")->openFile(FileSystem::OpenRead), FileProblemException);
    TS_ASSERT_THROWS(fs.openDirectory("d//")->getDirectoryEntries(), FileProblemException);

    // Success: file name variations
    TS_ASSERT_THROWS_NOTHING(fs.openFile("/d/f1", FileSystem::OpenRead));
    TS_ASSERT_THROWS_NOTHING(fs.openFile("/d/./f1", FileSystem::OpenRead));
    TS_ASSERT_THROWS_NOTHING(fs.openFile("./d/f1", FileSystem::OpenRead));
    TS_ASSERT_THROWS_NOTHING(fs.openFile("d/f1", FileSystem::OpenRead));
    TS_ASSERT_THROWS_NOTHING(fs.openDirectory("/d")->getDirectoryEntries());
    TS_ASSERT_THROWS_NOTHING(fs.openDirectory("d")->getDirectoryEntries());
    TS_ASSERT_THROWS_NOTHING(fs.openDirectory("d/")->getDirectoryEntries());
}

/** Test enumerating a directory. */
void
TestIoInternalFileSystem::testEnum()
{
    afl::io::InternalFileSystem fs;
    prepare(fs);

    // Open directory
    Ref<Directory> dir = fs.openDirectory("/d");
    TS_ASSERT_EQUALS(dir->getTitle(), "d");
    TS_ASSERT_EQUALS(dir->getDirectoryName(), "/d");
    TS_ASSERT(dir->getParentDirectory().get() != 0);

    // Retrieve content
    Ref<afl::base::Enumerator<Ptr<DirectoryEntry> > > it = dir->getDirectoryEntries();
    Ptr<DirectoryEntry> entry;
    bool got1 = false, got2 = false;
    while (it->getNextElement(entry)) {
        TS_ASSERT(entry.get() != 0);
        TS_ASSERT_EQUALS(entry->getFileType(), DirectoryEntry::tFile);
        TS_ASSERT_EQUALS(&*entry->openContainingDirectory(), &*dir);
        if (entry->getTitle() == "f1") {
            TS_ASSERT(!got1);
            TS_ASSERT_EQUALS(entry->getFileSize(), 3U);
            got1 = true;
        } else if (entry->getTitle() == "f2") {
            TS_ASSERT(!got2);
            TS_ASSERT_EQUALS(entry->getFileSize(), 3U);
            got2 = true;
        } else {
            TS_FAIL("unexpected name");
        }
    }
    TS_ASSERT(got1);
    TS_ASSERT(got2);
}

/** Test dealing with DirectoryEntry objects. */
void
TestIoInternalFileSystem::testEntry()
{
    afl::io::InternalFileSystem fs;
    prepare(fs);

    // DirectoryEntry for a directory
    Ref<DirectoryEntry> e = fs.openDirectory("/")->getDirectoryEntryByName("d");
    TS_ASSERT_EQUALS(e->getFileType(), DirectoryEntry::tDirectory);
    TS_ASSERT_EQUALS(e->getTitle(), "d");
    TS_ASSERT_THROWS_NOTHING(e->openDirectory());
    TS_ASSERT_THROWS(e->openFile(FileSystem::OpenRead), FileProblemException);
    TS_ASSERT_THROWS(e->setFlag(DirectoryEntry::Hidden, true), FileProblemException);

    // DirectoryEntry for a file
    Ref<DirectoryEntry> f = fs.openDirectory("/d")->getDirectoryEntryByName("f1");
    TS_ASSERT_EQUALS(f->getFileType(), DirectoryEntry::tFile);
    TS_ASSERT_EQUALS(f->getTitle(), "f1");
    TS_ASSERT_EQUALS(f->getFileSize(), 3U);
    TS_ASSERT_THROWS(f->openDirectory(), FileProblemException);
    TS_ASSERT_THROWS_NOTHING(f->openFile(FileSystem::OpenRead));
    TS_ASSERT_THROWS(f->setFlag(DirectoryEntry::Hidden, true), FileProblemException);

    // DirectoryEntry for a nonexistant item
    Ref<DirectoryEntry> g = fs.openDirectory("/")->getDirectoryEntryByName("x");
    TS_ASSERT_EQUALS(g->getFileType(), DirectoryEntry::tUnknown);
    TS_ASSERT_EQUALS(g->getTitle(), "x");
    TS_ASSERT_EQUALS(g->getFileSize(), 0U);
    TS_ASSERT_THROWS(g->openDirectory(), FileProblemException);
    TS_ASSERT_THROWS(g->openFile(FileSystem::OpenRead), FileProblemException);
    TS_ASSERT_THROWS(g->renameTo("y"), FileProblemException);
    TS_ASSERT_THROWS(g->erase(), FileProblemException);
    TS_ASSERT_THROWS(g->setFlag(DirectoryEntry::Hidden, true), FileProblemException);

    // Error: bad name
    TS_ASSERT_THROWS(fs.openDirectory("/")->getDirectoryEntryByName("/"), FileProblemException);
    TS_ASSERT_THROWS(fs.openDirectory("/")->getDirectoryEntryByName(""), FileProblemException);
    TS_ASSERT_THROWS(fs.openDirectory("/")->getDirectoryEntryByName("."), FileProblemException);
    TS_ASSERT_THROWS(fs.openDirectory("/")->getDirectoryEntryByName("a/b"), FileProblemException);
}

/** Test erase. */
void
TestIoInternalFileSystem::testErase()
{
    afl::io::InternalFileSystem fs;
    prepare(fs);

    // Try to erase "d": fails because it's not empty
    Ref<DirectoryEntry> e = fs.openDirectory("/")->getDirectoryEntryByName("d");
    TS_ASSERT_THROWS(e->erase(), FileProblemException);
    TS_ASSERT_EQUALS(e->eraseNT(), false);

    // Erase the content; exercises parallel iteration and erasure
    Ref<afl::base::Enumerator<Ptr<DirectoryEntry> > > it = e->openDirectory()->getDirectoryEntries();
    Ptr<DirectoryEntry> entry;
    while (it->getNextElement(entry)) {
        TS_ASSERT_THROWS_NOTHING(entry->erase());
    }

    // "d" can not be erased
    TS_ASSERT_THROWS_NOTHING(e->erase());
}

/** Test rename. */
void
TestIoInternalFileSystem::testRename()
{
    // Rename directory
    {
        afl::io::InternalFileSystem fs;
        prepare(fs);
        TS_ASSERT_THROWS_NOTHING(fs.openDirectory("/")->getDirectoryEntryByName("d")->renameTo("e"));
        TS_ASSERT_THROWS_NOTHING(fs.openFile("/e/f1", FileSystem::OpenRead));
    }

    // Null-rename directory
    {
        afl::io::InternalFileSystem fs;
        prepare(fs);
        TS_ASSERT_THROWS_NOTHING(fs.openDirectory("/")->getDirectoryEntryByName("d")->renameTo("d"));
        TS_ASSERT_THROWS_NOTHING(fs.openFile("/d/f1", FileSystem::OpenRead));
    }

    // Rename file
    {
        afl::io::InternalFileSystem fs;
        prepare(fs);
        TS_ASSERT_THROWS_NOTHING(fs.openDirectory("/d")->getDirectoryEntryByName("f1")->renameTo("q"));
        TS_ASSERT_THROWS_NOTHING(fs.openFile("/d/q", FileSystem::OpenRead));
        TS_ASSERT_THROWS(fs.openFile("/d/f1", FileSystem::OpenRead), FileProblemException);
    }

    // Rename file, overwriting
    {
        afl::io::InternalFileSystem fs;
        prepare(fs);
        TS_ASSERT_THROWS_NOTHING(fs.openDirectory("/d")->getDirectoryEntryByName("f1")->renameTo("f2"));
        TS_ASSERT_THROWS(fs.openFile("/d/f1", FileSystem::OpenRead), FileProblemException);

        Ref<afl::io::Stream> f = fs.openFile("/d/f2", FileSystem::OpenRead);
        uint8_t tmp[3];
        TS_ASSERT_EQUALS(f->read(tmp), 3U);
        TS_ASSERT_EQUALS(afl::string::fromBytes(tmp), "one");
    }

    // Null-rename file
    {
        afl::io::InternalFileSystem fs;
        prepare(fs);
        TS_ASSERT_THROWS_NOTHING(fs.openDirectory("/d")->getDirectoryEntryByName("f1")->renameTo("f1"));
        TS_ASSERT_THROWS_NOTHING(fs.openFile("/d/f1", FileSystem::OpenRead));
    }

    // Error: rename directory, file in the way
    {
        afl::io::InternalFileSystem fs;
        prepare(fs);
        fs.openFile("/e", FileSystem::Create);
        TS_ASSERT_THROWS(fs.openDirectory("/")->getDirectoryEntryByName("d")->renameTo("e"), FileProblemException);
        TS_ASSERT_THROWS_NOTHING(fs.openFile("/d/f1", FileSystem::OpenRead));
        TS_ASSERT_THROWS_NOTHING(fs.openFile("/e", FileSystem::OpenRead));
    }

    // Error: rename file, directory in the way
    {
        afl::io::InternalFileSystem fs;
        prepare(fs);
        fs.openDirectory("/d")->getDirectoryEntryByName("x")->createAsDirectory();
        TS_ASSERT_THROWS(fs.openDirectory("/d")->getDirectoryEntryByName("f1")->renameTo("x"), FileProblemException);
        TS_ASSERT_THROWS_NOTHING(fs.openFile("/d/f1", FileSystem::OpenRead));
    }
}

/** Test open(Create) variations. */
void
TestIoInternalFileSystem::testCreate()
{
    afl::io::InternalFileSystem fs;
    prepare(fs);

    // Error cases
    TS_ASSERT_THROWS(fs.openFile("/d", FileSystem::Create), FileProblemException);
    TS_ASSERT_THROWS(fs.openFile("/d", FileSystem::CreateNew), FileProblemException);
    TS_ASSERT_THROWS(fs.openFile("/d/f1", FileSystem::CreateNew), FileProblemException);

    // Success case
    TS_ASSERT_THROWS_NOTHING(fs.openFile("/d/f1", FileSystem::Create));
    TS_ASSERT_EQUALS(fs.openDirectory("d")->getDirectoryEntryByName("f1")->getFileSize(), 0U);
    TS_ASSERT_EQUALS(fs.openDirectory("d")->getDirectoryEntryByName("f1")->getFileType(), DirectoryEntry::tFile);

    // Success case
    TS_ASSERT_THROWS_NOTHING(fs.openFile("/d/f3", FileSystem::CreateNew));
    TS_ASSERT_EQUALS(fs.openDirectory("d")->getDirectoryEntryByName("f3")->getFileType(), DirectoryEntry::tFile);
}

/** Test createDirectory(). */
void
TestIoInternalFileSystem::testCreateDirectory()
{
    afl::io::InternalFileSystem fs;
    prepare(fs);

    // Error: already exists
    TS_ASSERT_THROWS(fs.createDirectory("/d"), FileProblemException);
    TS_ASSERT_THROWS(fs.createDirectory("/d/f1"), FileProblemException);
    TS_ASSERT_THROWS(fs.createDirectory("/"), FileProblemException);
    TS_ASSERT_THROWS(fs.createDirectory("."), FileProblemException);
    TS_ASSERT_THROWS(fs.createDirectory(""), FileProblemException);

    // Success
    TS_ASSERT_THROWS_NOTHING(fs.createDirectory("/d/sub"));
    TS_ASSERT_THROWS_NOTHING(fs.openDirectory("/d/sub"));
}

