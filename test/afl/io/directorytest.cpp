/**
  *  \file test/afl/io/directorytest.cpp
  *  \brief Test for afl::io::Directory
  */

#include "afl/io/directory.hpp"

#include <cstdio>
#include <stdexcept>
#include "afl/except/assertionfailedexception.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/io/directoryentry.hpp"
#include "afl/io/filesystem.hpp"
#include "afl/io/internalfilesystem.hpp"
#include "afl/io/stream.hpp"
#include "afl/string/string.hpp"
#include "afl/test/testrunner.hpp"

namespace {
    const char FILE_NAME1[] = "__file01.dat";
    const char FILE_NAME2[] = "__file02.dat";
    const char DIR_NAME[] = "__dir03";

    /* Validate that a file exists / does not exist. */
    void validateFile(afl::test::Assert a, afl::io::Directory& dir, const char* fileName, bool mustExist)
    {
        // Pass 1: try by iterating
        bool seenTheFile = false;
        afl::base::Ref<afl::base::Enumerator<afl::base::Ptr<afl::io::DirectoryEntry> > > en = dir.getDirectoryEntries();
        a.check("getDirectoryEntries", en.asPtr().get() != 0);

        afl::base::Ptr<afl::io::DirectoryEntry> entry;
        while (en->getNextElement(entry)) {
            a.check("getNextElement", entry.get() != 0);

            // Use case-insensitive compare; the filesystem might not be case-preserving.
            if (afl::string::strCaseCompare(entry->getTitle(), fileName) == 0) {
                seenTheFile = true;
                a.check("getFileType", entry->getFileType() == afl::io::DirectoryEntry::tFile);
            }
        }
        a.checkEqual("seenTheFile", seenTheFile, mustExist);

        // Pass 2: try direct lookup
        entry = dir.getDirectoryEntryByName(fileName).asPtr();
        a.check("getDirectoryEntryByName", entry.get() != 0);
        if (mustExist) {
            a.check("getFileType", entry->getFileType() == afl::io::DirectoryEntry::tFile);
        } else {
            a.check("getFileType", entry->getFileType() == afl::io::DirectoryEntry::tUnknown);
        }
    }

    class Environment {
     public:
        Environment()
            { cleanup(); }
        ~Environment()
            { cleanup(); }
     private:
        void cleanup()
            {
                std::remove(FILE_NAME1);
                std::remove(FILE_NAME2);
            }
    };
}

/** Interface test. */
AFL_TEST_NOARG("afl.io.Directory:interface")
{
    class Tester : public afl::io::Directory {
     public:
        virtual afl::base::Ref<afl::io::DirectoryEntry> getDirectoryEntryByName(String_t /*name*/)
            { throw std::runtime_error("no ref"); }
        virtual afl::base::Ref<afl::base::Enumerator<afl::base::Ptr<afl::io::DirectoryEntry> > > getDirectoryEntries()
            { throw std::runtime_error("no ref"); }
        virtual afl::base::Ptr<afl::io::Directory> getParentDirectory()
            { return 0; }
        virtual String_t getDirectoryName()
            { return String_t(); }
        virtual String_t getTitle()
            { return String_t(); }
    };
    Tester t;
}

/** Simple test. */
AFL_TEST("afl.io.Directory:basic-sequence", a)
{
    Environment env;
    afl::io::FileSystem& fs = afl::io::FileSystem::getInstance();

    {
        // Open current directory
        afl::base::Ref<afl::io::Directory> dir = fs.openDirectory(fs.getWorkingDirectoryName());
        a.check("open working directory", dir.asPtr().get() != 0);

        // Must not have too messed-up properties
        a.check("working directory name", dir->getDirectoryName() != "");
        a.check("working directory title", dir->getTitle() != "");

        // Get an entry
        afl::base::Ref<afl::io::DirectoryEntry> entry = dir->getDirectoryEntryByName("hi");

        // Check entry's properties. We cannot assume anything about them,
        // but it shouldn't crash or something.
        entry->getFileType();
        entry->getFileSize();
        entry->getLinkText();

        // Get that entry's parent. Must be same again.
        afl::base::Ref<afl::io::Directory> dir2 = entry->openContainingDirectory();
        a.check("working directory absolute path name", fs.getAbsolutePathName(dir->getDirectoryName()) == fs.getAbsolutePathName(dir2->getDirectoryName()));
    }

    {
        // Try making a file.
        afl::base::Ref<afl::io::Directory> dir = fs.openDirectory(fs.getWorkingDirectoryName());

        {
            afl::base::Ref<afl::io::Stream> file = dir->openFile(FILE_NAME1, afl::io::FileSystem::Create);
            uint8_t data[] = { 1, 2, 3 };
            file->write(data);
            a.checkEqual("file getSize", file->getSize(), 3U);
        }

        // Get and validate entry
        afl::base::Ref<afl::io::DirectoryEntry> entry = dir->getDirectoryEntryByName(FILE_NAME1);
        a.check("entry getFileType", entry->getFileType() == afl::io::DirectoryEntry::tFile);
        a.checkEqual("entry getFileSize", entry->getFileSize(), 3U);

        // Now get rid of the file
        entry->erase();
        a.check("erased getFileType", entry->getFileType() == afl::io::DirectoryEntry::tUnknown);
        AFL_CHECK_THROWS(a("open erased file"), entry->openFile(afl::io::FileSystem::OpenRead), afl::except::FileProblemException);
    }
}

/** Test creation and deletion. */
AFL_TEST("afl.io.Directory:create-delete-sequence", a)
{
    // ex ArchDirectoryTestSuite::testCreationAndDeletion
    Environment env;
    afl::io::FileSystem& fs = afl::io::FileSystem::getInstance();
    afl::base::Ref<afl::io::Directory> dir = fs.openDirectory(fs.getWorkingDirectoryName());

    // Get some directory entry. Since a file system is modifiable, this must always be possible.
    afl::base::Ptr<afl::io::DirectoryEntry> entry;
    AFL_CHECK_SUCCEEDS(a("01. getDirectoryEntryByName"), entry = dir->getDirectoryEntryByName(DIR_NAME).asPtr());
    a.check("02. ptr", entry.get() != 0);
    AFL_CHECK_SUCCEEDS(a("03. getFileType"), entry->getFileType());

    // Create it
    AFL_CHECK_SUCCEEDS(a("11. createAsDirectory"), entry->createAsDirectory());
    a.check("12. getFileType", entry->getFileType() == afl::io::DirectoryEntry::tDirectory);

    // Create a file in it
    afl::base::Ptr<afl::io::Directory> subdir;
    AFL_CHECK_SUCCEEDS(a("21. openDirectory"), subdir = entry->openDirectory().asPtr());
    a.check("22. ptr", subdir.get() != 0);
    AFL_CHECK_SUCCEEDS(a("23. openFile"), subdir->openFile("test", fs.Create));
    AFL_CHECK_SUCCEEDS(a("24. erase"), subdir->erase("test"));

    // Erase directory again
    AFL_CHECK_SUCCEEDS(a("31. erase"), entry->erase());

    // Try same thing with Unicode name
    AFL_CHECK_SUCCEEDS(a("41. getDirectoryEntryByName"), entry = dir->getDirectoryEntryByName("__03b\xc3\xa4r").asPtr());
    a.check("42. ptr", entry.get() != 0);
    AFL_CHECK_SUCCEEDS(a("43. getFileType"), entry->getFileType());
    try {
        entry->createAsDirectory();
        a.check("44. getFileType", entry->getFileType() == afl::io::DirectoryEntry::tDirectory);
        AFL_CHECK_SUCCEEDS(a("45. erase"), entry->erase());
    }
    catch (afl::except::AssertionFailedException& e) {
        throw;
    }
    catch (...) {
        /* Maybe the OS doesn't allow this name */
    }
}

/** Test directory access. */
AFL_TEST("afl.io.Directory:directory-access", a)
{
    Environment env;
    afl::io::FileSystem& fs = afl::io::FileSystem::getInstance();
    afl::base::Ref<afl::io::Directory> dir = fs.openDirectory(fs.getWorkingDirectoryName());
    a.check("01. openDirectory", dir.asPtr().get() != 0);

    // Create a file
    afl::base::Ref<afl::io::DirectoryEntry> entry = dir->getDirectoryEntryByName(FILE_NAME1);
    a.check("02. getDirectoryEntryByName", entry.asPtr().get() != 0);

    {
        afl::base::Ref<afl::io::Stream> s = entry->openFile(fs.Create);
        a.check("03. openFile", s.asPtr().get() != 0);

        static const uint8_t DATA[] = {'f','o','o'};
        size_t written = s->write(DATA);
        a.checkEqual("04. write", written, 3U);
    }

    // Check that the file is actually there
    validateFile(a("05. file 1"), *dir, FILE_NAME1, true);
    validateFile(a("06. file 2"), *dir, FILE_NAME2, false);

    // Rename the file
    AFL_CHECK_SUCCEEDS(a("11. renameTo"), entry->renameTo(FILE_NAME2));

    // Directory entry continues to refer to old name, which cannot be opened
    AFL_CHECK_THROWS(a("12. openFile"), entry->openFile(fs.OpenRead), afl::except::FileProblemException);

    // New entry that refers to new file
    AFL_CHECK_SUCCEEDS(a("21. getDirectoryEntryByName"), entry.reset(*dir->getDirectoryEntryByName(FILE_NAME2)));
    a.check("22. ptr", entry.asPtr().get() != 0);
    a.checkEqual("23. getFileSize", entry->getFileSize(), 3U);
    a.check("24. getFileType", entry->getFileType() == afl::io::DirectoryEntry::tFile);

    validateFile(a("25. file 1"), *dir, FILE_NAME1, false);
    validateFile(a("26. file 2"), *dir, FILE_NAME2, true);

    // Try reading the file
    {
        afl::base::Ref<afl::io::Stream> s = entry->openFile(fs.OpenRead);
        a.check("31. openFile", s.asPtr().get() != 0);

        uint8_t tmp[10];
        size_t read = s->read(tmp);
        a.checkEqual("32. read", read, 3U);
        a.checkEqual("33. content", tmp[0], 'f');
        a.checkEqual("34. content", tmp[1], 'o');
        a.checkEqual("35. content", tmp[2], 'o');
    }

    // Erase the file
    AFL_CHECK_SUCCEEDS(a("41. erase"), entry->erase());

    // Directory entry continues to refer to old name, which cannot be opened
    AFL_CHECK_THROWS(a("42. openFile"), entry->openFile(fs.OpenRead), afl::except::FileProblemException);

    validateFile(a("43. file 1"), *dir, FILE_NAME1, false);
    validateFile(a("44. file 2"), *dir, FILE_NAME2, false);
}

/** Test directory access: subdirectories. */
AFL_TEST("afl.io.Directory:subdir-access", a)
{
    afl::io::FileSystem& fs = afl::io::FileSystem::getInstance();
    afl::base::Ref<afl::io::Directory> dir = fs.openDirectory(fs.getWorkingDirectoryName());
    a.check("01. openDirectory", dir.asPtr().get() != 0);

    // Create a directory
    AFL_CHECK_SUCCEEDS(a("02. createAsDirectory"), dir->getDirectoryEntryByName(DIR_NAME)->createAsDirectory());

    // Try opening it; verify content (must be empty)
    {
        afl::base::Ref<afl::io::Directory> subdir = dir->openDirectory(DIR_NAME);
        afl::base::Ref<afl::base::Enumerator<afl::base::Ptr<afl::io::DirectoryEntry> > > content = subdir->getDirectoryEntries();
        afl::base::Ptr<afl::io::DirectoryEntry> tmp;
        a.checkEqual("11. getNextElement", content->getNextElement(tmp), false);
    }

    // Remove it
    a.checkEqual("21. eraseNT", dir->eraseNT(DIR_NAME), true);
}

/** Test parent access.
    Using parents to go up must terminate. */
AFL_TEST("afl.io.Directory:getParentDirectory", a)
{
    afl::io::FileSystem& fs = afl::io::FileSystem::getInstance();
    afl::base::Ptr<afl::io::Directory> dir = fs.openDirectory(fs.getWorkingDirectoryName()).asPtr();
    a.check("openDirectory", dir.get() != 0);

    int level = 0;
    while (dir.get() != 0) {
        ++level;
        a.check("acceptable level", level < 100);
        dir = dir->getParentDirectory();
    }
}

/** Test eraseContentRecursively(). */
AFL_TEST("afl.io.Directory:eraseContentRecursively", a)
{
    // Set up
    afl::io::InternalFileSystem fs;
    fs.openDirectory("/")->getDirectoryEntryByName("d")->createAsDirectory();
    fs.openDirectory("/d")->getDirectoryEntryByName("a")->openFile(afl::io::FileSystem::Create);
    fs.openDirectory("/d")->getDirectoryEntryByName("b")->createAsDirectory();
    fs.openDirectory("/d")->getDirectoryEntryByName("c")->openFile(afl::io::FileSystem::Create);
    fs.openDirectory("/d/b")->getDirectoryEntryByName("x")->createAsDirectory();
    fs.openDirectory("/d/b")->getDirectoryEntryByName("y")->openFile(afl::io::FileSystem::Create);

    // Execute
    fs.openDirectory("/d")->eraseContentRecursively();

    // Verify
    afl::base::Ptr<afl::io::DirectoryEntry> e;
    a.check("getNextElement", !fs.openDirectory("/d")->getDirectoryEntries()->getNextElement(e));
}

/** Test eraseContentRecursively() on live system. */
AFL_TEST("afl.io.Directory:eraseContentRecursively:live", a)
{
    // Create
    afl::io::FileSystem& fs = afl::io::FileSystem::getInstance();
    afl::base::Ref<afl::io::Directory> dir = fs.openDirectory(fs.getWorkingDirectoryName());
    AFL_CHECK_SUCCEEDS(a("createAsDirectory"), dir->getDirectoryEntryByName(DIR_NAME)->createAsDirectory());

    // Populate
    {
        afl::base::Ref<afl::io::Directory> subdir = dir->openDirectory(DIR_NAME);
        subdir->openFile("a", afl::io::FileSystem::Create);
        subdir->openFile("b", afl::io::FileSystem::Create);
        subdir->openFile("c", afl::io::FileSystem::Create);
    }

    // Directory is un-removable now
    a.checkEqual("eraseNT 1", dir->eraseNT(DIR_NAME), false);

    // Remove content
    dir->openDirectory(DIR_NAME)->eraseContentRecursively();

    // Remove it
    a.checkEqual("eraseNT 2", dir->eraseNT(DIR_NAME), true);
}
