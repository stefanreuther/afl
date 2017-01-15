/**
  *  \file u/t_io_internaldirectory.cpp
  *  \brief Test for afl::io::InternalDirectory
  */

#include "afl/io/internaldirectory.hpp"

#include "u/t_io.hpp"
#include "afl/io/internalstream.hpp"
#include "afl/io/filesystem.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/io/directoryentry.hpp"
#include "afl/string/string.hpp"
#include "afl/base/countof.hpp"

using afl::base::Ptr;
using afl::base::Ref;
using afl::io::InternalDirectory;
using afl::io::InternalStream;
using afl::io::FileSystem;
using afl::io::Stream;
using afl::io::Directory;
using afl::io::DirectoryEntry;

/** Test basic MemoryDirectory API. */
void
TestIoInternalDirectory::testBaseAPI()
{
    // Create directory
    Ref<InternalDirectory> md = InternalDirectory::create("tester");
    TS_ASSERT(&md.get() != 0);
    TS_ASSERT_EQUALS(md->getTitle(), "tester");
    TS_ASSERT_EQUALS(md->getDirectoryName(), "");

    // Check some operations on blank FS:
    TS_ASSERT_EQUALS(md->eraseStream("foo"), false);
    TS_ASSERT_EQUALS(md->renameStream("foo", "bar"), false);
    TS_ASSERT(md->getStream("foo").get() == 0);

    // Create a file
    Ref<InternalStream> ms = *new InternalStream();
    md->addStream("foo", ms);
    TS_ASSERT(&ms.get() != 0);
    TS_ASSERT_EQUALS(ms->getSize(), 0U);
    TS_ASSERT_EQUALS(ms->getPos(), 0U);
    TS_ASSERT_EQUALS(ms.asPtr(), md->getStream("foo"));

    // Create another file
    Ref<InternalStream> ms2 = *new InternalStream();
    md->addStream("bar", ms2);
    TS_ASSERT(&ms2.get() != 0);
    TS_ASSERT_EQUALS(ms2->getSize(), 0U);
    TS_ASSERT_EQUALS(ms2->getPos(), 0U);
    TS_ASSERT_EQUALS(ms2.asPtr(), md->getStream("bar"));

    // Rename file
    TS_ASSERT_EQUALS(md->renameStream("foo", "bar"), true);
    TS_ASSERT(md->getStream("foo").get() == 0);
    TS_ASSERT_EQUALS(md->getStream("bar"), ms.asPtr());

    // Erase file
    TS_ASSERT_EQUALS(md->eraseStream("foo"), false);
    TS_ASSERT_EQUALS(md->eraseStream("bar"), true);
    TS_ASSERT_EQUALS(md->eraseStream("bar"), false);
    TS_ASSERT(md->getStream("foo").get() == 0);
    TS_ASSERT(md->getStream("bar").get() == 0);
}

/** Test that we can create and access files using regular APIs. */
void
TestIoInternalDirectory::testCreation()
{
    // Create directory
    Ref<Directory> md = InternalDirectory::create("tester");
    TS_ASSERT_EQUALS(md->getTitle(), "tester");

    // Open fails for nonexistant file
    TS_ASSERT_EQUALS(md->openFileNT("foo", FileSystem::OpenRead), Ptr<Stream>(0));
    TS_ASSERT_THROWS(md->openFile("foo", FileSystem::OpenRead), afl::except::FileProblemException);
    TS_ASSERT_EQUALS(md->eraseNT("foo"), false);
    TS_ASSERT_THROWS(md->erase("foo"), afl::except::FileProblemException);

    TS_ASSERT_DIFFERS(md->getDirectoryEntryByName("foo").asPtr(), Ptr<DirectoryEntry>(0));
    TS_ASSERT_THROWS(md->getDirectoryEntryByName("foo")->renameTo("bar"), afl::except::FileProblemException);

    // Test creation
    Ref<Stream> s = md->openFile("foo", FileSystem::Create);
    TS_ASSERT(&s.get() != 0);
    TS_ASSERT_EQUALS(s->getPos(), 0U);
    TS_ASSERT_EQUALS(s->getSize(), 0U);
    TS_ASSERT_EQUALS(s->write(afl::string::toBytes("hello")), 5U);
    TS_ASSERT_EQUALS(s->getPos(), 5U);
    TS_ASSERT_EQUALS(s->getSize(), 5U);

    // Cannot re-create
    TS_ASSERT_THROWS(md->openFile("foo", FileSystem::CreateNew), afl::except::FileProblemException);

    // Test re-opening / reading
    uint8_t tmp[10];
    Ref<Stream> s2 = md->openFile("foo", FileSystem::OpenRead);
    TS_ASSERT(&s2.get() != 0);
    TS_ASSERT_EQUALS(s2->getPos(), 0U);
    TS_ASSERT_EQUALS(s2->getSize(), 5U);
    TS_ASSERT_EQUALS(s2->read(tmp), 5U);
    TS_ASSERT_SAME_DATA(tmp, "hello", 5);
    TS_ASSERT_EQUALS(s2->getPos(), 5U);
    TS_ASSERT_EQUALS(s2->getSize(), 5U);

    Ref<Stream> s3 = md->openFile("foo", FileSystem::OpenWrite);
    TS_ASSERT(&s3.get() != 0);
    TS_ASSERT_EQUALS(s3->getPos(), 0U);
    TS_ASSERT_EQUALS(s3->getSize(), 5U);
    TS_ASSERT_EQUALS(s3->read(afl::base::Memory<uint8_t>::unsafeCreate(tmp, 3)), 3U);
    TS_ASSERT_SAME_DATA(tmp, "hel", 3);
    TS_ASSERT_EQUALS(s3->getPos(), 3U);
    TS_ASSERT_EQUALS(s3->getSize(), 5U);

    // Update through file pointer 3
    TS_ASSERT_EQUALS(s3->write(afl::string::toBytes("p me!")), 5U);
    TS_ASSERT_EQUALS(s3->getPos(), 8U);
    TS_ASSERT_EQUALS(s3->getSize(), 8U);

    // All other file sizes also changed, but file pointers stayed
    TS_ASSERT_EQUALS(s->getSize(), 8U);
    TS_ASSERT_EQUALS(s2->getSize(), 8U);
    TS_ASSERT_EQUALS(s->getPos(), 5U);
    TS_ASSERT_EQUALS(s2->getPos(), 5U);

    // Check that we can read the enlarged file through an old handle
    TS_ASSERT_EQUALS(s2->read(tmp), 3U);
    TS_ASSERT_EQUALS(s2->getPos(), 8U);
    TS_ASSERT_SAME_DATA(tmp, "me!", 3U);

    // Check that creation resets file to zero size
    Ref<Stream> s4 = md->openFile("foo", FileSystem::Create);
    TS_ASSERT_EQUALS(s4->getSize(), 0U);
    TS_ASSERT_EQUALS(s3->getSize(), 0U);
    TS_ASSERT_EQUALS(s2->getSize(), 0U);
    TS_ASSERT_EQUALS(s->getSize(), 0U);
    TS_ASSERT_EQUALS(s2->read(tmp), 0U);
}

/** Test iteration. */
void
TestIoInternalDirectory::testIteration()
{
    // Create directory
    Ref<Directory> md = InternalDirectory::create("tester");
    TS_ASSERT_EQUALS(md->getTitle(), "tester");

    // Must be initially empty
    Ptr<afl::base::Enumerator<Ptr<DirectoryEntry> > > e = md->getDirectoryEntries().asPtr();
    Ptr<DirectoryEntry> result;
    TS_ASSERT(e.get() != 0);
    TS_ASSERT(!e->getNextElement(result));

    // Create file with some content
    {
        Ref<Stream> s = md->openFile("a", FileSystem::Create);
        TS_ASSERT_EQUALS(s->write(afl::string::toBytes("hallo")), 5U);
    }

    // Must now contain one item with correct specs
    e = md->getDirectoryEntries().asPtr();
    TS_ASSERT(e.get() != 0);
    Ptr<DirectoryEntry> entry;
    TS_ASSERT(e->getNextElement(entry));
    TS_ASSERT(entry.get() != 0);
    TS_ASSERT_EQUALS(entry->getTitle(), "a");
    TS_ASSERT_EQUALS(entry->getPathName(), "");      // empty, it's virtual
    TS_ASSERT_EQUALS(entry->getFileSize(), 5U);
    TS_ASSERT_EQUALS(entry->getFileType(), DirectoryEntry::tFile);

    // Entry must be open-able as file
    {
        Ptr<Stream> s;
        TS_ASSERT_THROWS_NOTHING(s = entry->openFile(FileSystem::OpenRead).asPtr());
        TS_ASSERT(s.get() != 0);
        TS_ASSERT_EQUALS(s->getSize(), 5U);
    }

    // Entry must not be open-able as directory
    TS_ASSERT_THROWS(entry->openDirectory(), afl::except::FileProblemException);

    // Entry must be linked to directory
    TS_ASSERT(&entry->openContainingDirectory().get() == &md.get());

    // Directory must not contain more elements
    TS_ASSERT(!e->getNextElement(entry));
}

/** Test concurrent iteration and deletion. */
void
TestIoInternalDirectory::testConcurrency()
{
    static const char*const names[] = { "a", "b", "c", "d" };

    // Perform some iteration; while doing so, delete files.
    for (size_t toDelete = 0; toDelete < countof(names); ++toDelete) {
        // Create test setup
        Ref<Directory> md = InternalDirectory::create("tester");
        for (size_t i = 0; i < countof(names); ++i) {
            md->openFile(names[i], FileSystem::Create);
        }

        // Create iterator
        Ref<afl::base::Enumerator<Ptr<DirectoryEntry> > > e = md->getDirectoryEntries();
        Ptr<DirectoryEntry> entry;
        TS_ASSERT(e->getNextElement(entry));
        TS_ASSERT(entry.get() != 0);
        TS_ASSERT(e->getNextElement(entry));

        // Delete an element
        TS_ASSERT_THROWS_NOTHING(md->erase(names[toDelete]));

        // Move the iterator to the end. Must not crash.
        do {
            TS_ASSERT(entry.get() != 0);
        } while (e->getNextElement(entry));
    }
}
