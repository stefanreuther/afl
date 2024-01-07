/**
  *  \file test/afl/io/temporarydirectorytest.cpp
  *  \brief Test for afl::io::TemporaryDirectory
  */

#include "afl/io/temporarydirectory.hpp"

#include <memory>
#include "afl/except/fileproblemexception.hpp"
#include "afl/io/internalfilesystem.hpp"
#include "afl/test/testrunner.hpp"

/** Test creating TemporaryDirectory on live system. */
AFL_TEST("afl.io.TemporaryDirectory:live", a)
{
    // Create a temporary directory
    afl::io::FileSystem& fs = afl::io::FileSystem::getInstance();
    afl::base::Ref<afl::io::Directory> dir = fs.openDirectory(fs.getWorkingDirectoryName());
    std::auto_ptr<afl::io::TemporaryDirectory> testee(new afl::io::TemporaryDirectory(dir));

    // Directory must have a file system path
    a.check("must have FS path", testee->get()->getDirectoryName() != "");

    // Directory must be child of desired parent
    String_t name = testee->get()->getTitle();
    a.check("must be child of parent", dir->getDirectoryEntryByName(name)->getFileType() == afl::io::DirectoryEntry::tDirectory);

    // Create a file...
    testee->get()->openFile("a", afl::io::FileSystem::Create);

    // Remove the temporary directory
    testee.reset();
    a.check("must be removed", dir->getDirectoryEntryByName(name)->getFileType() == afl::io::DirectoryEntry::tUnknown);
}

/** Test creating TemporaryDirectory on InternalFileSystem (otherwise, same test as testLive). */
AFL_TEST("afl.io.TemporaryDirectory:internal", a)
{
    // Create a temporary directory
    afl::io::InternalFileSystem fs;
    afl::base::Ref<afl::io::Directory> dir = fs.openDirectory(fs.getWorkingDirectoryName());
    std::auto_ptr<afl::io::TemporaryDirectory> testee(new afl::io::TemporaryDirectory(dir));

    // Directory must have a file system path
    a.check("must have FS path", testee->get()->getDirectoryName() != "");

    // Directory must be child of desired parent
    String_t name = testee->get()->getTitle();
    a.check("must be child of parent", dir->getDirectoryEntryByName(name)->getFileType() == afl::io::DirectoryEntry::tDirectory);

    // Create a file...
    testee->get()->openFile("a", afl::io::FileSystem::Create);

    // Remove the temporary directory
    testee.reset();
    a.check("must be removed", dir->getDirectoryEntryByName(name)->getFileType() == afl::io::DirectoryEntry::tUnknown);
}

/** Test failure to create TemporaryDirectory.
    The virtual root does not allow creation of subdirectories. */
AFL_TEST("afl.io.TemporaryDirectory:fail-on-root", a)
{
    afl::io::InternalFileSystem fs;
    std::auto_ptr<afl::io::TemporaryDirectory> testee;
    AFL_CHECK_THROWS(a, testee.reset(new afl::io::TemporaryDirectory(fs.openRootDirectory())), afl::except::FileProblemException);
}

/** Test failure to clean up TemporaryDirectory. */
AFL_TEST("afl.io.TemporaryDirectory:failure-to-erase", a)
{
    // Create a temporary directory
    afl::io::InternalFileSystem fs;
    afl::base::Ref<afl::io::Directory> dir = fs.openDirectory(fs.getWorkingDirectoryName());
    std::auto_ptr<afl::io::TemporaryDirectory> testee(new afl::io::TemporaryDirectory(dir));

    // Remove it
    String_t name = testee->get()->getTitle();
    AFL_CHECK_SUCCEEDS(a("manual erase"), dir->getDirectoryEntryByName(name)->erase());

    // Remove the TemporaryDirectory object
    AFL_CHECK_SUCCEEDS(a("cleanup must not fail"), testee.reset());
}
