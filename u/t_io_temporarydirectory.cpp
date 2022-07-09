/**
  *  \file u/t_io_temporarydirectory.cpp
  *  \brief Test for afl::io::TemporaryDirectory
  */

#include <memory>
#include "afl/io/temporarydirectory.hpp"

#include "t_io.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/io/internalfilesystem.hpp"

/** Test creating TemporaryDirectory on live system. */
void
TestIoTemporaryDirectory::testLive()
{
    // Create a temporary directory
    afl::io::FileSystem& fs = afl::io::FileSystem::getInstance();
    afl::base::Ref<afl::io::Directory> dir = fs.openDirectory(fs.getWorkingDirectoryName());
    std::auto_ptr<afl::io::TemporaryDirectory> testee(new afl::io::TemporaryDirectory(dir));

    // Directory must have a file system path
    TS_ASSERT(testee->get()->getDirectoryName() != "");

    // Directory must be child of desired parent
    String_t name = testee->get()->getTitle();
    TS_ASSERT_EQUALS(dir->getDirectoryEntryByName(name)->getFileType(), afl::io::DirectoryEntry::tDirectory);

    // Create a file...
    testee->get()->openFile("a", afl::io::FileSystem::Create);

    // Remove the temporary directory
    testee.reset();
    TS_ASSERT_EQUALS(dir->getDirectoryEntryByName(name)->getFileType(), afl::io::DirectoryEntry::tUnknown);
}

/** Test creating TemporaryDirectory on InternalFileSystem (otherwise, same test as testLive). */
void
TestIoTemporaryDirectory::testInternal()
{
    // Create a temporary directory
    afl::io::InternalFileSystem fs;
    afl::base::Ref<afl::io::Directory> dir = fs.openDirectory(fs.getWorkingDirectoryName());
    std::auto_ptr<afl::io::TemporaryDirectory> testee(new afl::io::TemporaryDirectory(dir));

    // Directory must have a file system path
    TS_ASSERT(testee->get()->getDirectoryName() != "");

    // Directory must be child of desired parent
    String_t name = testee->get()->getTitle();
    TS_ASSERT_EQUALS(dir->getDirectoryEntryByName(name)->getFileType(), afl::io::DirectoryEntry::tDirectory);

    // Create a file...
    testee->get()->openFile("a", afl::io::FileSystem::Create);

    // Remove the temporary directory
    testee.reset();
    TS_ASSERT_EQUALS(dir->getDirectoryEntryByName(name)->getFileType(), afl::io::DirectoryEntry::tUnknown);
}

/** Test failure to create TemporaryDirectory.
    The virtual root does not allow creation of subdirectories. */
void
TestIoTemporaryDirectory::testFail()
{
    afl::io::InternalFileSystem fs;
    std::auto_ptr<afl::io::TemporaryDirectory> testee;
    TS_ASSERT_THROWS(testee.reset(new afl::io::TemporaryDirectory(fs.openRootDirectory())), afl::except::FileProblemException);
}

/** Test failure to clean up TemporaryDirectory. */
void
TestIoTemporaryDirectory::testFailErase()
{
    // Create a temporary directory
    afl::io::InternalFileSystem fs;
    afl::base::Ref<afl::io::Directory> dir = fs.openDirectory(fs.getWorkingDirectoryName());
    std::auto_ptr<afl::io::TemporaryDirectory> testee(new afl::io::TemporaryDirectory(dir));

    // Remove it
    String_t name = testee->get()->getTitle();
    TS_ASSERT_THROWS_NOTHING(dir->getDirectoryEntryByName(name)->erase());

    // Remove the TemporaryDirectory object
    TS_ASSERT_THROWS_NOTHING(testee.reset());
}

