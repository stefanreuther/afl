/**
  *  \file test/afl/io/internalfilesystemtest.cpp
  *  \brief Test for afl::io::InternalFileSystem
  */

#include "afl/io/internalfilesystem.hpp"

#include "afl/except/fileproblemexception.hpp"
#include "afl/io/directoryentry.hpp"
#include "afl/test/testrunner.hpp"

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
AFL_TEST("afl.io.InternalFileSystem:file-names", a)
{
    afl::io::InternalFileSystem fs;

    // getWorkingDirectoryName
    a.check("getWorkingDirectoryName 1", fs.getWorkingDirectoryName() != "");
    a.check("getWorkingDirectoryName 2", fs.isAbsolutePathName(fs.getWorkingDirectoryName()));

    // isPathSeparator
    a.check("isPathSeparator 1", fs.isPathSeparator('/'));
    a.check("isPathSeparator 2", !fs.isPathSeparator('\\'));
    a.check("isPathSeparator 3", !fs.isPathSeparator('\0'));
    a.check("isPathSeparator 4", !fs.isPathSeparator(':'));

    // isAbsolutePathName
    a.check("isAbsolutePathName 1", !fs.isAbsolutePathName(""));
    a.check("isAbsolutePathName 2", fs.isAbsolutePathName(fs.getAbsolutePathName("x")));
    a.check("isAbsolutePathName 3", fs.isAbsolutePathName("/foo"));
    a.check("isAbsolutePathName 4", fs.isAbsolutePathName("//foo"));          // syntactically absolute, but invalid for accessing files
    a.check("isAbsolutePathName 5", fs.isAbsolutePathName("///foo/bar"));     // syntactically absolute, but invalid for accessing files
    a.check("isAbsolutePathName 6", !fs.isAbsolutePathName("a"));
    a.check("isAbsolutePathName 7", !fs.isAbsolutePathName("a/"));
    a.check("isAbsolutePathName 8", !fs.isAbsolutePathName("a///"));          // syntactically absolute, but invalid for accessing files
    a.check("isAbsolutePathName 9", !fs.isAbsolutePathName("."));

    // makePathName
    a.checkEqual("makePathName 1", fs.makePathName("a", "b"), "a/b");
    a.checkEqual("makePathName 2", fs.makePathName("a/", "b"), "a/b");
    a.checkEqual("makePathName 3", fs.makePathName("", "b"), "b");
    a.checkEqual("makePathName 4", fs.makePathName("a", ""), "a/");

    a.checkEqual("makePathName 5", fs.makePathName("a", "/"), "/");
    a.checkEqual("makePathName 6", fs.makePathName("a/", "/foo"), "/foo");

    // getCanonicalPathName
    a.checkEqual("getCanonicalPathName 01", fs.getCanonicalPathName("a/b/c"), "a/b/c");
    a.checkEqual("getCanonicalPathName 02", fs.getCanonicalPathName("a///c"), "a/c");
    a.checkEqual("getCanonicalPathName 03", fs.getCanonicalPathName("a/./c"), "a/c");
    a.checkEqual("getCanonicalPathName 04", fs.getCanonicalPathName("a/../c"), "c");

    a.checkEqual("getCanonicalPathName 05", fs.getCanonicalPathName("/a/b/c"), "/a/b/c");
    a.checkEqual("getCanonicalPathName 06", fs.getCanonicalPathName("/a///c"), "/a/c");
    a.checkEqual("getCanonicalPathName 07", fs.getCanonicalPathName("/a/./c"), "/a/c");
    a.checkEqual("getCanonicalPathName 08", fs.getCanonicalPathName("/a/../c"), "/c");

    a.checkEqual("getCanonicalPathName 09", fs.getCanonicalPathName("../a/b/c"), "../a/b/c");

    a.checkEqual("getCanonicalPathName 10", fs.getCanonicalPathName(""), ".");
    a.checkEqual("getCanonicalPathName 11", fs.getCanonicalPathName("."), ".");
    a.checkEqual("getCanonicalPathName 12", fs.getCanonicalPathName("/"), "/");
    a.checkEqual("getCanonicalPathName 13", fs.getCanonicalPathName("/."), "/");

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

    // Reading a directory: opening "." or "/" works, but check that null is correctly rejected
    AFL_CHECK_SUCCEEDS(a("openDirectory 1"), fs.openDirectory(".")->getDirectoryEntries());
    AFL_CHECK_SUCCEEDS(a("openDirectory 2"), fs.openDirectory("/")->getDirectoryEntries());
    AFL_CHECK_THROWS(a("openDirectory 3"), fs.openDirectory(String_t(".\0Q", 3))->getDirectoryEntries(), FileProblemException);
    AFL_CHECK_THROWS(a("openDirectory 4"), fs.openDirectory(String_t("/\0Q", 3))->getDirectoryEntries(), FileProblemException);
}

/** Test iteration of roots.
    Modelled after TestIoFileSystem::testRoot() */
AFL_TEST("afl.io.InternalFileSystem:openRootDirectory", a)
{
    afl::io::InternalFileSystem fs;

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

    // Enumerate content: must be one
    Ref<afl::base::Enumerator<Ptr<DirectoryEntry> > > entries = root->getDirectoryEntries();
    Ptr<DirectoryEntry> entry;
    a.check("11. getNextElement", entries->getNextElement(entry));

    // Verify the entry
    a.check("12. ptr", entry.get() != 0);
    a.check("13. getTitle", !entry->getTitle().empty());
    a.checkEqual("14. openContainingDirectory", &*entry->openContainingDirectory(), &*root);
    a.check("15. getFileType", entry->getFileType() == DirectoryEntry::tDirectory);
    a.check("16. getFlags", entry->getFlags().contains(DirectoryEntry::Link));
    a.checkEqual("17. getLinkText", entry->getLinkText(), "/");

    // Cannot be modified
    AFL_CHECK_THROWS(a("21. renameTo"), entry->renameTo("foo"), FileProblemException);
    AFL_CHECK_THROWS(a("22. erase"), entry->erase(), FileProblemException);
    AFL_CHECK_THROWS(a("23. createAsDirectory"), entry->createAsDirectory(), FileProblemException);
    AFL_CHECK_THROWS(a("24. setFlag"), entry->setFlag(DirectoryEntry::Hidden, true), FileProblemException);

    // Can open the content as directory
    a.checkEqual("31. getDirectoryName", entry->openDirectory()->getDirectoryName(), "/");
    a.check("32. getParentDirectory", entry->openDirectory()->getParentDirectory().get() == 0);

    // Cannot open the content as file
    AFL_CHECK_THROWS(a("33. openFile"), entry->openFile(FileSystem::OpenRead), FileProblemException);

    // No more entries
    a.check("34. getNextElement", !entries->getNextElement(entry));
}

/** Test handling of content. */
AFL_TEST("afl.io.InternalFileSystem:content", a)
{
    afl::io::InternalFileSystem fs;
    prepare(fs);

    // Error: createAsDirectory when directory exists
    AFL_CHECK_THROWS(a("01. createAsDirectory"), fs.openDirectory("/")->getDirectoryEntryByName("d")->createAsDirectory(), FileProblemException);

    // Error: createAsDirectory when file exists
    AFL_CHECK_THROWS(a("02. createAsDirectory"), fs.openDirectory("/d")->getDirectoryEntryByName("f1")->createAsDirectory(), FileProblemException);

    // Error: openFile(Create) when directory exists
    AFL_CHECK_THROWS(a("03. Create"), fs.openDirectory("/")->getDirectoryEntryByName("d")->openFile(FileSystem::Create), FileProblemException);
    AFL_CHECK_THROWS(a("04. Create"), fs.openFile("/d", FileSystem::Create), FileProblemException);

    // Error: openFile(CreateNew) when file exists
    AFL_CHECK_THROWS(a("05. CreateNew"), fs.openDirectory("/d")->getDirectoryEntryByName("f1")->openFile(FileSystem::CreateNew), FileProblemException);
    AFL_CHECK_THROWS(a("06. CreateNew"), fs.openFile("/d/f1", FileSystem::CreateNew), FileProblemException);

    // Error: openFile(OpenRead) when directory exists
    AFL_CHECK_THROWS(a("07. OpenRead"), fs.openDirectory("/")->getDirectoryEntryByName("d")->openFile(FileSystem::OpenRead), FileProblemException);
    AFL_CHECK_THROWS(a("08. OpenRead"), fs.openFile("/d", FileSystem::OpenRead), FileProblemException);

    // Error: openFile when container does not exist
    AFL_CHECK_THROWS(a("09. OpenRead"), fs.openFile("/q/a", FileSystem::OpenRead), FileProblemException);
    AFL_CHECK_THROWS(a("10. Create"), fs.openFile("/q/a", FileSystem::Create), FileProblemException);

    // Error: openFile when file does not exist
    AFL_CHECK_THROWS(a("11. OpenRead"), fs.openFile("/d/a", FileSystem::OpenRead), FileProblemException);

    // Error: file name syntax error
    AFL_CHECK_THROWS(a("12. openRead syntax"), fs.openFile("/d//f1", FileSystem::OpenRead), FileProblemException);
    AFL_CHECK_THROWS(a("13. openRead syntax"), fs.openFile("d//f1", FileSystem::OpenRead), FileProblemException);
    AFL_CHECK_THROWS(a("14. openRead syntax"), fs.openFile("d/f1/", FileSystem::OpenRead), FileProblemException);
    AFL_CHECK_THROWS(a("15. openRead syntax"), fs.openDirectory("/")->getDirectoryEntryByName("d/f1")->openFile(FileSystem::OpenRead), FileProblemException);
    AFL_CHECK_THROWS(a("16. openRead syntax"), fs.openDirectory("d//")->getDirectoryEntries(), FileProblemException);

    // Success: file name variations
    AFL_CHECK_SUCCEEDS(a("17. openFile"), fs.openFile("/d/f1", FileSystem::OpenRead));
    AFL_CHECK_SUCCEEDS(a("18. openFile"), fs.openFile("/d/./f1", FileSystem::OpenRead));
    AFL_CHECK_SUCCEEDS(a("19. openFile"), fs.openFile("./d/f1", FileSystem::OpenRead));
    AFL_CHECK_SUCCEEDS(a("20. openFile"), fs.openFile("d/f1", FileSystem::OpenRead));
    AFL_CHECK_SUCCEEDS(a("21. openDirectory"), fs.openDirectory("/d")->getDirectoryEntries());
    AFL_CHECK_SUCCEEDS(a("22. openDirectory"), fs.openDirectory("d")->getDirectoryEntries());
    AFL_CHECK_SUCCEEDS(a("23. openDirectory"), fs.openDirectory("d/")->getDirectoryEntries());
}

/** Test enumerating a directory. */
AFL_TEST("afl.io.InternalFileSystem:getDirectoryEntries", a)
{
    afl::io::InternalFileSystem fs;
    prepare(fs);

    // Open directory
    Ref<Directory> dir = fs.openDirectory("/d");
    a.checkEqual("01. getTitle", dir->getTitle(), "d");
    a.checkEqual("02. getDirectoryEntries", dir->getDirectoryName(), "/d");
    a.check("03. getParentDirectory", dir->getParentDirectory().get() != 0);

    // Retrieve content
    Ref<afl::base::Enumerator<Ptr<DirectoryEntry> > > it = dir->getDirectoryEntries();
    Ptr<DirectoryEntry> entry;
    bool got1 = false, got2 = false;
    while (it->getNextElement(entry)) {
        a.check("11. ptr", entry.get() != 0);
        a.check("12. getFileType", entry->getFileType() == DirectoryEntry::tFile);
        a.checkEqual("13. openContainingDirectory", &*entry->openContainingDirectory(), &*dir);
        if (entry->getTitle() == "f1") {
            a.check("14. got1", !got1);
            a.checkEqual("15. getFileSize", entry->getFileSize(), 3U);
            got1 = true;
        } else if (entry->getTitle() == "f2") {
            a.check("16. got2", !got2);
            a.checkEqual("17. getFileSize", entry->getFileSize(), 3U);
            got2 = true;
        } else {
            a.fail("unexpected name");
        }
    }
    a.check("21. got1", got1);
    a.check("22. got2", got2);
}

/** Test dealing with DirectoryEntry objects. */
AFL_TEST("afl.io.InternalFileSystem:DirectoryEntry", a)
{
    afl::io::InternalFileSystem fs;
    prepare(fs);

    // DirectoryEntry for a directory
    Ref<DirectoryEntry> e = fs.openDirectory("/")->getDirectoryEntryByName("d");
    a.check("01. getFileType", e->getFileType() == DirectoryEntry::tDirectory);
    a.checkEqual("02. getTitle", e->getTitle(), "d");
    AFL_CHECK_SUCCEEDS(a("03. openDirectory"), e->openDirectory());
    AFL_CHECK_THROWS(a("04. openFile"), e->openFile(FileSystem::OpenRead), FileProblemException);
    AFL_CHECK_THROWS(a("05. setFlag"), e->setFlag(DirectoryEntry::Hidden, true), FileProblemException);

    // DirectoryEntry for a file
    Ref<DirectoryEntry> f = fs.openDirectory("/d")->getDirectoryEntryByName("f1");
    a.check("11. getFileType", f->getFileType() == DirectoryEntry::tFile);
    a.checkEqual("12. getTitle", f->getTitle(), "f1");
    a.checkEqual("13. getFileSize", f->getFileSize(), 3U);
    AFL_CHECK_THROWS(a("14. openDirectory"), f->openDirectory(), FileProblemException);
    AFL_CHECK_SUCCEEDS(a("15. openFile"), f->openFile(FileSystem::OpenRead));
    AFL_CHECK_THROWS(a("16. setFlag"), f->setFlag(DirectoryEntry::Hidden, true), FileProblemException);

    // DirectoryEntry for a nonexistant item
    Ref<DirectoryEntry> g = fs.openDirectory("/")->getDirectoryEntryByName("x");
    a.check("21. getFileType", g->getFileType() == DirectoryEntry::tUnknown);
    a.checkEqual("22. getTitle", g->getTitle(), "x");
    a.checkEqual("23. getFileSize", g->getFileSize(), 0U);
    AFL_CHECK_THROWS(a("24. openDirectory"), g->openDirectory(), FileProblemException);
    AFL_CHECK_THROWS(a("25. openFile"), g->openFile(FileSystem::OpenRead), FileProblemException);
    AFL_CHECK_THROWS(a("26. renameTo"), g->renameTo("y"), FileProblemException);
    AFL_CHECK_THROWS(a("27. erase"), g->erase(), FileProblemException);
    AFL_CHECK_THROWS(a("28. setFlag"), g->setFlag(DirectoryEntry::Hidden, true), FileProblemException);

    // Error: bad name
    AFL_CHECK_THROWS(a("31. getDirectoryEntryByName"), fs.openDirectory("/")->getDirectoryEntryByName("/"), FileProblemException);
    AFL_CHECK_THROWS(a("32. getDirectoryEntryByName"), fs.openDirectory("/")->getDirectoryEntryByName(""), FileProblemException);
    AFL_CHECK_THROWS(a("33. getDirectoryEntryByName"), fs.openDirectory("/")->getDirectoryEntryByName("."), FileProblemException);
    AFL_CHECK_THROWS(a("34. getDirectoryEntryByName"), fs.openDirectory("/")->getDirectoryEntryByName("a/b"), FileProblemException);
}

/** Test erase. */
AFL_TEST("afl.io.InternalFileSystem:erase", a)
{
    afl::io::InternalFileSystem fs;
    prepare(fs);

    // Try to erase "d": fails because it's not empty
    Ref<DirectoryEntry> e = fs.openDirectory("/")->getDirectoryEntryByName("d");
    AFL_CHECK_THROWS(a("01. erase"), e->erase(), FileProblemException);
    a.checkEqual("02. eraseNT", e->eraseNT(), false);

    // Erase the content; exercises parallel iteration and erasure
    Ref<afl::base::Enumerator<Ptr<DirectoryEntry> > > it = e->openDirectory()->getDirectoryEntries();
    Ptr<DirectoryEntry> entry;
    while (it->getNextElement(entry)) {
        AFL_CHECK_SUCCEEDS(a("11. erase"), entry->erase());
    }

    // "d" can not be erased
    AFL_CHECK_SUCCEEDS(a("21. erase"), e->erase());
}

/** Test rename. */
// Rename directory
AFL_TEST("afl.io.InternalFileSystem:rename:directory", a)
{
    afl::io::InternalFileSystem fs;
    prepare(fs);
    AFL_CHECK_SUCCEEDS(a("rename"), fs.openDirectory("/")->getDirectoryEntryByName("d")->renameTo("e"));
    AFL_CHECK_SUCCEEDS(a("openFile"), fs.openFile("/e/f1", FileSystem::OpenRead));
}

// Null-rename directory
AFL_TEST("afl.io.InternalFileSystem:rename:directory-noop", a)
{
    afl::io::InternalFileSystem fs;
    prepare(fs);
    AFL_CHECK_SUCCEEDS(a("rename"), fs.openDirectory("/")->getDirectoryEntryByName("d")->renameTo("d"));
    AFL_CHECK_SUCCEEDS(a("openFile"), fs.openFile("/d/f1", FileSystem::OpenRead));
}

// Rename file
AFL_TEST("afl.io.InternalFileSystem:rename:file", a)
{
    afl::io::InternalFileSystem fs;
    prepare(fs);
    AFL_CHECK_SUCCEEDS(a("rename"), fs.openDirectory("/d")->getDirectoryEntryByName("f1")->renameTo("q"));
    AFL_CHECK_SUCCEEDS(a("openFile new"), fs.openFile("/d/q", FileSystem::OpenRead));
    AFL_CHECK_THROWS(a("openFile old"), fs.openFile("/d/f1", FileSystem::OpenRead), FileProblemException);
}

// Rename file, overwriting
AFL_TEST("afl.io.InternalFileSystem:rename:file-overwrite", a)
{
    afl::io::InternalFileSystem fs;
    prepare(fs);
    AFL_CHECK_SUCCEEDS(a("rename"), fs.openDirectory("/d")->getDirectoryEntryByName("f1")->renameTo("f2"));
    AFL_CHECK_THROWS(a("openFile old"), fs.openFile("/d/f1", FileSystem::OpenRead), FileProblemException);

    Ref<afl::io::Stream> f = fs.openFile("/d/f2", FileSystem::OpenRead);
    uint8_t tmp[3];
    a.checkEqual("read", f->read(tmp), 3U);
    a.checkEqual("content", afl::string::fromBytes(tmp), "one");
}

// Null-rename file
AFL_TEST("afl.io.InternalFileSystem:rename:file-noop", a)
{
    afl::io::InternalFileSystem fs;
    prepare(fs);
    AFL_CHECK_SUCCEEDS(a("rename"), fs.openDirectory("/d")->getDirectoryEntryByName("f1")->renameTo("f1"));
    AFL_CHECK_SUCCEEDS(a("openFile"), fs.openFile("/d/f1", FileSystem::OpenRead));
}

// Error: rename directory, file in the way
AFL_TEST("afl.io.InternalFileSystem:rename:directory-conflict", a)
{
    afl::io::InternalFileSystem fs;
    prepare(fs);
    fs.openFile("/e", FileSystem::Create);
    AFL_CHECK_THROWS(a("rename"), fs.openDirectory("/")->getDirectoryEntryByName("d")->renameTo("e"), FileProblemException);
    AFL_CHECK_SUCCEEDS(a("openFile 1"), fs.openFile("/d/f1", FileSystem::OpenRead));
    AFL_CHECK_SUCCEEDS(a("openFile 2"), fs.openFile("/e", FileSystem::OpenRead));
}

// Error: rename file, directory in the way
AFL_TEST("afl.io.InternalFileSystem:rename:file-conflict", a)
{
    afl::io::InternalFileSystem fs;
    prepare(fs);
    fs.openDirectory("/d")->getDirectoryEntryByName("x")->createAsDirectory();
    AFL_CHECK_THROWS(a("rename"), fs.openDirectory("/d")->getDirectoryEntryByName("f1")->renameTo("x"), FileProblemException);
    AFL_CHECK_SUCCEEDS(a("openFile"), fs.openFile("/d/f1", FileSystem::OpenRead));
}

/** Test open(Create) variations. */
AFL_TEST("afl.io.InternalFileSystem:create", a)
{
    afl::io::InternalFileSystem fs;
    prepare(fs);

    // Error cases
    AFL_CHECK_THROWS(a("01. overwrite dir"), fs.openFile("/d", FileSystem::Create), FileProblemException);
    AFL_CHECK_THROWS(a("02. overwrite dir"), fs.openFile("/d", FileSystem::CreateNew), FileProblemException);
    AFL_CHECK_THROWS(a("03. overwrite file"), fs.openFile("/d/f1", FileSystem::CreateNew), FileProblemException);

    // Success case
    AFL_CHECK_SUCCEEDS(a("11. create"), fs.openFile("/d/f1", FileSystem::Create));
    a.checkEqual("12. getFileSize", fs.openDirectory("d")->getDirectoryEntryByName("f1")->getFileSize(), 0U);
    a.check("13. getFileType", fs.openDirectory("d")->getDirectoryEntryByName("f1")->getFileType() == DirectoryEntry::tFile);

    // Success case
    AFL_CHECK_SUCCEEDS(a("21. create"), fs.openFile("/d/f3", FileSystem::CreateNew));
    a.check("22. getFileType", fs.openDirectory("d")->getDirectoryEntryByName("f3")->getFileType() == DirectoryEntry::tFile);
}

/** Test createDirectory(). */
AFL_TEST("afl.io.InternalFileSystem:createDirectory", a)
{
    afl::io::InternalFileSystem fs;
    prepare(fs);

    // Error: already exists
    AFL_CHECK_THROWS(a("dir exists"),   fs.createDirectory("/d"), FileProblemException);
    AFL_CHECK_THROWS(a("file exists"),  fs.createDirectory("/d/f1"), FileProblemException);
    AFL_CHECK_THROWS(a("root exists"),  fs.createDirectory("/"), FileProblemException);
    AFL_CHECK_THROWS(a("dot exists"),   fs.createDirectory("."), FileProblemException);
    AFL_CHECK_THROWS(a("invalid name"), fs.createDirectory(""), FileProblemException);

    // Success
    AFL_CHECK_SUCCEEDS(a("create success"), fs.createDirectory("/d/sub"));
    AFL_CHECK_SUCCEEDS(a("open success"),   fs.openDirectory("/d/sub"));
}
