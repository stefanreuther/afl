/**
  *  \file u/t_io_directory.cpp
  *  \brief Test for afl::io::Directory
  */

#include "afl/io/directory.hpp"

#include <stdexcept>
#include <cstdio>
#include "u/t_io.hpp"
#include "afl/io/filesystem.hpp"
#include "afl/io/stream.hpp"
#include "afl/io/directoryentry.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/string/string.hpp"

namespace {
    const char FILE_NAME1[] = "__file01.dat";
    const char FILE_NAME2[] = "__file02.dat";

    /* Validate that a file exists / does not exist. */
    void validateFile(afl::io::Directory& dir, const char* fileName, bool mustExist)
    {
        // Pass 1: try by iterating
        bool seenTheFile = false;
        afl::base::Ref<afl::base::Enumerator<afl::base::Ptr<afl::io::DirectoryEntry> > > en = dir.getDirectoryEntries();
        TS_ASSERT(&en.get() != 0);

        afl::base::Ptr<afl::io::DirectoryEntry> entry;
        while (en->getNextElement(entry)) {
            TS_ASSERT(entry.get() != 0);

            // Use case-insensitive compare; the filesystem might not be case-preserving.
            if (afl::string::strCaseCompare(entry->getTitle(), fileName) == 0) {
                seenTheFile = true;
                TS_ASSERT_EQUALS(entry->getFileType(), afl::io::DirectoryEntry::tFile);
            }
        }
        TS_ASSERT_EQUALS(seenTheFile, mustExist);

        // Pass 2: try direct lookup
        entry = dir.getDirectoryEntryByName(fileName).asPtr();
        TS_ASSERT(entry.get() != 0);
        if (mustExist) {
            TS_ASSERT_EQUALS(entry->getFileType(), afl::io::DirectoryEntry::tFile);
        } else {
            TS_ASSERT_EQUALS(entry->getFileType(), afl::io::DirectoryEntry::tUnknown);
        }
    }
}

void
TestIoDirectory::setUp()
{
    std::remove(FILE_NAME1);
    std::remove(FILE_NAME2);
}

void
TestIoDirectory::tearDown()
{
    TestIoDirectory::setUp();
}

/** Interface test. */
void
TestIoDirectory::testInterface()
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
void
TestIoDirectory::testIt()
{
    afl::io::FileSystem& fs = afl::io::FileSystem::getInstance();

    {
        // Open current directory
        afl::base::Ref<afl::io::Directory> dir = fs.openDirectory(fs.getWorkingDirectoryName());
        TS_ASSERT(&dir.get() != 0);

        // Must not have too messed-up properties
        TS_ASSERT(dir->getDirectoryName() != "");
        TS_ASSERT(dir->getTitle() != "");

        // Get an entry
        afl::base::Ref<afl::io::DirectoryEntry> entry = dir->getDirectoryEntryByName("hi");

        // Check entry's properties. We cannot assume anything about them,
        // but it shouldn't crash or something.
        entry->getFileType();
        entry->getFileSize();
        entry->getLinkText();

        // Get that entry's parent. Must be same again.
        afl::base::Ref<afl::io::Directory> dir2 = entry->openContainingDirectory();
        TS_ASSERT(fs.getAbsolutePathName(dir->getDirectoryName()) == fs.getAbsolutePathName(dir2->getDirectoryName()));
    }

    {
        // Try making a file.
        afl::base::Ref<afl::io::Directory> dir = fs.openDirectory(fs.getWorkingDirectoryName());

        {
            afl::base::Ref<afl::io::Stream> file = dir->openFile(FILE_NAME1, afl::io::FileSystem::Create);
            uint8_t data[] = { 1, 2, 3 };
            file->write(data);
            TS_ASSERT_EQUALS(file->getSize(), 3U);
        }

        // Get and validate entry
        afl::base::Ref<afl::io::DirectoryEntry> entry = dir->getDirectoryEntryByName(FILE_NAME1);
        TS_ASSERT_EQUALS(entry->getFileType(), afl::io::DirectoryEntry::tFile);
        TS_ASSERT_EQUALS(entry->getFileSize(), 3U);

        // Now get rid of the file
        entry->erase();
        TS_ASSERT_EQUALS(entry->getFileType(), afl::io::DirectoryEntry::tUnknown);
        TS_ASSERT_THROWS(entry->openFile(afl::io::FileSystem::OpenRead), afl::except::FileProblemException);
    }
}

/** Test creation and deletion. */
void
TestIoDirectory::testCreationAndDeletion()
{
    // ex ArchDirectoryTestSuite::testCreationAndDeletion
    afl::io::FileSystem& fs = afl::io::FileSystem::getInstance();
    afl::base::Ref<afl::io::Directory> dir = fs.openDirectory(fs.getWorkingDirectoryName());

    // Get some directory entry. Since a file system is modifiable, this must always be possible.
    afl::base::Ptr<afl::io::DirectoryEntry> entry;
    TS_ASSERT_THROWS_NOTHING(entry = dir->getDirectoryEntryByName("__03dir").asPtr());
    TS_ASSERT(entry.get() != 0);
    TS_ASSERT_THROWS_NOTHING(entry->getFileType());

    // Create it
    TS_ASSERT_THROWS_NOTHING(entry->createAsDirectory());
    TS_ASSERT_EQUALS(entry->getFileType(), afl::io::DirectoryEntry::tDirectory);

    // Create a file in it
    afl::base::Ptr<afl::io::Directory> subdir;
    TS_ASSERT_THROWS_NOTHING(subdir = entry->openDirectory().asPtr());
    TS_ASSERT(subdir.get() != 0);
    TS_ASSERT_THROWS_NOTHING(subdir->openFile("test", fs.Create));
    TS_ASSERT_THROWS_NOTHING(subdir->erase("test"));

    // Erase directory again
    TS_ASSERT_THROWS_NOTHING(entry->erase());

    // Try same thing with Unicode name
    TS_ASSERT_THROWS_NOTHING(entry = dir->getDirectoryEntryByName("__03b\xc3\xa4r").asPtr());
    TS_ASSERT(entry.get() != 0);
    TS_ASSERT_THROWS_NOTHING(entry->getFileType());
    try {
        entry->createAsDirectory();
        TS_ASSERT_EQUALS(entry->getFileType(), afl::io::DirectoryEntry::tDirectory);
        TS_ASSERT_THROWS_NOTHING(entry->erase());
    }
    catch (...) {
        /* Maybe the OS doesn't allow this name */
    }
}

/** Test directory access. */
void
TestIoDirectory::testDirectoryAccess()
{
    afl::io::FileSystem& fs = afl::io::FileSystem::getInstance();
    afl::base::Ref<afl::io::Directory> dir = fs.openDirectory(fs.getWorkingDirectoryName());
    TS_ASSERT(&dir.get() != 0);

    // Create a file
    afl::base::Ref<afl::io::DirectoryEntry> entry = dir->getDirectoryEntryByName(FILE_NAME1);
    TS_ASSERT(&entry.get() != 0);

    {
        afl::base::Ref<afl::io::Stream> s = entry->openFile(fs.Create);
        TS_ASSERT(&s.get() != 0);

        static const uint8_t DATA[] = {'f','o','o'};
        size_t written = s->write(DATA);
        TS_ASSERT_EQUALS(written, 3U);
    }

    // Check that the file is actually there
    validateFile(*dir, FILE_NAME1, true);
    validateFile(*dir, FILE_NAME2, false);

    // Rename the file
    TS_ASSERT_THROWS_NOTHING(entry->renameTo(FILE_NAME2));

    // Directory entry continues to refer to old name, which cannot be opened
    TS_ASSERT_THROWS(entry->openFile(fs.OpenRead), afl::except::FileProblemException);

    // New entry that refers to new file
    TS_ASSERT_THROWS_NOTHING(entry.reset(*dir->getDirectoryEntryByName(FILE_NAME2)));
    TS_ASSERT(&entry.get() != 0);
    TS_ASSERT_EQUALS(entry->getFileSize(), 3U);
    TS_ASSERT_EQUALS(entry->getFileType(), afl::io::DirectoryEntry::tFile);

    validateFile(*dir, FILE_NAME1, false);
    validateFile(*dir, FILE_NAME2, true);

    // Try reading the file
    {
        afl::base::Ref<afl::io::Stream> s = entry->openFile(fs.OpenRead);
        TS_ASSERT(&s.get() != 0);

        uint8_t tmp[10];
        size_t read = s->read(tmp);
        TS_ASSERT_EQUALS(read, 3U);
        TS_ASSERT_SAME_DATA(tmp, "foo", 3U);
    }

    // Erase the file
    TS_ASSERT_THROWS_NOTHING(entry->erase());

    // Directory entry continues to refer to old name, which cannot be opened
    TS_ASSERT_THROWS(entry->openFile(fs.OpenRead), afl::except::FileProblemException);

    validateFile(*dir, FILE_NAME1, false);
    validateFile(*dir, FILE_NAME2, false);
}

