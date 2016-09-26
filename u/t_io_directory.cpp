/**
  *  \file u/t_io_directory.cpp
  *  \brief Test for afl::io::Directory
  */

#include "afl/io/directory.hpp"

#include "u/t_io.hpp"
#include "afl/io/filesystem.hpp"
#include "afl/io/stream.hpp"
#include "afl/io/directoryentry.hpp"
#include "afl/except/fileproblemexception.hpp"

/** Simple test. */
void
TestIoDirectory::testIt()
{
    afl::io::FileSystem& fs = afl::io::FileSystem::getInstance();

    {
        // Open current directory
        afl::base::Ptr<afl::io::Directory> dir = fs.openDirectory(fs.getWorkingDirectoryName());
        TS_ASSERT(dir.get() != 0);

        // Must not have too messed-up properties
        TS_ASSERT(dir->getDirectoryName() != "");
        TS_ASSERT(dir->getTitle() != "");

        // Get an entry
        afl::base::Ptr<afl::io::DirectoryEntry> entry = dir->getDirectoryEntryByName("hi");
        TS_ASSERT(entry.get() != 0);

        // Check entry's properties. We cannot assume anything about them,
        // but it shouldn't crash or something.
        entry->getFileType();
        entry->getFileSize();
        entry->getLinkText();

        // Get that entry's parent. Must be same again.
        afl::base::Ptr<afl::io::Directory> dir2 = entry->openContainingDirectory();
        TS_ASSERT(fs.getAbsolutePathName(dir->getDirectoryName()) == fs.getAbsolutePathName(dir2->getDirectoryName()));
    }

    {
        // Try making a file.
        static const char FILENAME[] = "__test.tmp";
        afl::base::Ptr<afl::io::Directory> dir = fs.openDirectory(fs.getWorkingDirectoryName());

        {
            afl::base::Ptr<afl::io::Stream> file = dir->openFile(FILENAME, afl::io::FileSystem::Create);
            uint8_t data[] = { 1, 2, 3 };
            file->write(data);
            TS_ASSERT_EQUALS(file->getSize(), 3U);
        }

        // Get and validate entry
        afl::base::Ptr<afl::io::DirectoryEntry> entry = dir->getDirectoryEntryByName(FILENAME);
        TS_ASSERT(entry.get() != 0);
        TS_ASSERT_EQUALS(entry->getFileType(), afl::io::DirectoryEntry::tFile);
        TS_ASSERT_EQUALS(entry->getFileSize(), 3U);

        // Now get rid of the file
        entry->erase();
        TS_ASSERT_EQUALS(entry->getFileType(), afl::io::DirectoryEntry::tUnknown);
        TS_ASSERT_THROWS(entry->openFile(afl::io::FileSystem::OpenRead), afl::except::FileProblemException);
    }
}
