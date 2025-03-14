/**
  *  \file test/afl/io/internaldirectorytest.cpp
  *  \brief Test for afl::io::InternalDirectory
  */

#include "afl/io/internaldirectory.hpp"

#include "afl/base/countof.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/io/directoryentry.hpp"
#include "afl/io/filesystem.hpp"
#include "afl/io/internalstream.hpp"
#include "afl/string/string.hpp"
#include "afl/test/testrunner.hpp"

using afl::base::ConstBytes_t;
using afl::base::Ptr;
using afl::base::Ref;
using afl::except::FileProblemException;
using afl::io::Directory;
using afl::io::DirectoryEntry;
using afl::io::FileSystem;
using afl::io::InternalDirectory;
using afl::io::InternalStream;
using afl::io::Stream;

/** Test basic InternalDirectory API. */
AFL_TEST("afl.io.InternalDirectory:basic", a)
{
    // Create directory
    Ref<InternalDirectory> md = InternalDirectory::create("tester");
    a.check("01. create", md.asPtr().get() != 0);
    a.checkEqual("02. getTitle", md->getTitle(), "tester");
    a.checkEqual("03. getDirectoryName", md->getDirectoryName(), "");

    // Check some operations on blank FS:
    a.checkEqual("11. eraseStream", md->eraseStream("foo"), false);
    a.checkEqual("12. renameStream", md->renameStream("foo", "bar"), false);
    a.checkNull("13. getStream", md->getStream("foo").get());

    // Create a file
    Ref<InternalStream> ms = *new InternalStream();
    md->addStream("foo", ms);
    a.checkNonNull("21. ptr", &ms.get());
    a.checkEqual("22. getSize", ms->getSize(), 0U);
    a.checkEqual("23. getPos", ms->getPos(), 0U);
    a.checkEqual("24. getStream", ms.asPtr().get(), md->getStream("foo").get());

    // Create another file
    Ref<InternalStream> ms2 = *new InternalStream();
    md->addStream("bar", ms2);
    a.checkNonNull("31. ptr", &ms2.get());
    a.checkEqual("32. getSize", ms2->getSize(), 0U);
    a.checkEqual("33. getPos", ms2->getPos(), 0U);
    a.checkEqual("34. getStream", ms2.asPtr().get(), md->getStream("bar").get());

    // Rename file
    a.checkEqual("41. renameStream", md->renameStream("foo", "bar"), true);
    a.checkNull("42. getStream", md->getStream("foo").get());
    a.checkEqual("43. getStream", md->getStream("bar").get(), ms.asPtr().get());

    // Erase file
    a.checkEqual("51. eraseStream", md->eraseStream("foo"), false);
    a.checkEqual("52. eraseStream", md->eraseStream("bar"), true);
    a.checkEqual("53. eraseStream", md->eraseStream("bar"), false);
    a.checkNull("54. getStream", md->getStream("foo").get());
    a.checkNull("55. getStream", md->getStream("bar").get());
}

/** Test that we can create and access files using regular APIs. */
AFL_TEST("afl.io.InternalDirectory:create", a)
{
    // Create directory
    Ref<Directory> md = InternalDirectory::create("tester");
    a.checkEqual("01. getTitle", md->getTitle(), "tester");

    // Open fails for nonexistant file
    a.checkNull("02. open", md->openFileNT("foo", FileSystem::OpenRead).get());
    AFL_CHECK_THROWS(a("03. open"), md->openFile("foo", FileSystem::OpenRead), FileProblemException);
    a.checkEqual("04. eraseNT", md->eraseNT("foo"), false);
    AFL_CHECK_THROWS(a("05. erase"), md->erase("foo"), FileProblemException);

    a.checkNonNull("06. getDirectoryEntryByName", md->getDirectoryEntryByName("foo").asPtr().get());
    AFL_CHECK_THROWS(a("07. getDirectoryEntryByName"), md->getDirectoryEntryByName("foo")->renameTo("bar"), FileProblemException);

    // Test creation
    Ref<Stream> s = md->openFile("foo", FileSystem::Create);
    a.check("11. addr", s.asPtr().get() != 0);
    a.checkEqual("12. getPos", s->getPos(), 0U);
    a.checkEqual("13. getSize", s->getSize(), 0U);
    a.checkEqual("14. write", s->write(afl::string::toBytes("hello")), 5U);
    a.checkEqual("15. getPos", s->getPos(), 5U);
    a.checkEqual("16. getSize", s->getSize(), 5U);

    // Cannot re-create
    AFL_CHECK_THROWS(a("21. create"), md->openFile("foo", FileSystem::CreateNew), FileProblemException);

    // Test re-opening / reading
    uint8_t tmp[10];
    Ref<Stream> s2 = md->openFile("foo", FileSystem::OpenRead);
    a.check("21. addr", s2.asPtr().get() != 0);
    a.checkEqual("22. getPos", s2->getPos(), 0U);
    a.checkEqual("23. getSize", s2->getSize(), 5U);
    a.checkEqual("24. read", s2->read(tmp), 5U);
    a.checkEqualContent("25. data", ConstBytes_t(tmp).trim(5), afl::string::toBytes("hello"));
    a.checkEqual("26. getPos", s2->getPos(), 5U);
    a.checkEqual("27. getSize", s2->getSize(), 5U);

    Ref<Stream> s3 = md->openFile("foo", FileSystem::OpenWrite);
    a.check("31. addr", s3.asPtr().get() != 0);
    a.checkEqual("32. getPos", s3->getPos(), 0U);
    a.checkEqual("33. getSize", s3->getSize(), 5U);
    a.checkEqual("34. read", s3->read(afl::base::Memory<uint8_t>::unsafeCreate(tmp, 3)), 3U);
    a.checkEqualContent("35. data", ConstBytes_t(tmp).trim(3), afl::string::toBytes("hel"));
    a.checkEqual("36. getPos", s3->getPos(), 3U);
    a.checkEqual("37. getSize", s3->getSize(), 5U);

    // Update through file pointer 3
    a.checkEqual("41. write", s3->write(afl::string::toBytes("p me!")), 5U);
    a.checkEqual("42. getPos", s3->getPos(), 8U);
    a.checkEqual("43. getSize", s3->getSize(), 8U);

    // All other file sizes also changed, but file pointers stayed
    a.checkEqual("51. getSize", s->getSize(), 8U);
    a.checkEqual("52. getSize", s2->getSize(), 8U);
    a.checkEqual("53. getPos", s->getPos(), 5U);
    a.checkEqual("54. getPos", s2->getPos(), 5U);

    // Check that we can read the enlarged file through an old handle
    a.checkEqual("61. read", s2->read(tmp), 3U);
    a.checkEqual("62. getPos", s2->getPos(), 8U);
    a.checkEqualContent("63. data", ConstBytes_t(tmp).trim(3), afl::string::toBytes("me!"));

    // Check that creation resets file to zero size
    Ref<Stream> s4 = md->openFile("foo", FileSystem::Create);
    a.checkEqual("71. getSize", s4->getSize(), 0U);
    // Re-creating the stream will discard the previous "master" stream,
    // which in turn invalidates all its children.
    a.checkEqual("72. getSize", s3->getSize(), 0U);
    a.checkEqual("73. getSize", s2->getSize(), 0U);
    a.checkEqual("74. getSize", s->getSize(), 0U);
    a.checkEqual("75. read", s2->read(tmp), 0U);
}

/** Test iteration. */
AFL_TEST("afl.io.InternalDirectory:iteration", a)
{
    // Create directory
    Ref<Directory> md = InternalDirectory::create("tester");
    a.checkEqual("01. getTitle", md->getTitle(), "tester");

    // Must be initially empty
    Ptr<afl::base::Enumerator<Ptr<DirectoryEntry> > > e = md->getDirectoryEntries().asPtr();
    Ptr<DirectoryEntry> result;
    a.check("11. ptr", e.get() != 0);
    a.check("12. getNextElement", !e->getNextElement(result));

    // Create file with some content
    {
        Ref<Stream> s = md->openFile("a", FileSystem::Create);
        a.checkEqual("21. write", s->write(afl::string::toBytes("hallo")), 5U);
    }

    // Must now contain one item with correct specs
    e = md->getDirectoryEntries().asPtr();
    a.check("31. getDirectoryEntries", e.get() != 0);
    Ptr<DirectoryEntry> entry;
    a.check("32. getNextElement", e->getNextElement(entry));
    a.check("33. ptr", entry.get() != 0);
    a.checkEqual("34. getTitle", entry->getTitle(), "a");
    a.checkEqual("35. getPathName", entry->getPathName(), "");      // empty, it's virtual
    a.checkEqual("36. getFileSize", entry->getFileSize(), 5U);
    a.check("37. getFileType", entry->getFileType() == DirectoryEntry::tFile);

    // Entry must be open-able as file
    {
        Ptr<Stream> s;
        AFL_CHECK_SUCCEEDS(a("41. openFile"), s = entry->openFile(FileSystem::OpenRead).asPtr());
        a.checkNonNull("42. ptr", s.get());
        a.checkEqual("43. getSize", s->getSize(), 5U);
    }

    // Entry must not be open-able as directory
    AFL_CHECK_THROWS(a("51. openDirectory"), entry->openDirectory(), FileProblemException);

    // Entry must be linked to directory
    a.check("61. openContainingDirectory", &entry->openContainingDirectory().get() == &md.get());

    // Directory must not contain more elements
    a.check("71. getNextElement", !e->getNextElement(entry));
}

/** Test concurrent iteration and deletion. */
AFL_TEST("afl.io.InternalDirectory:concurrent-iteration", a)
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
        a.check("01. getNextElement", e->getNextElement(entry));
        a.check("02. ptr", entry.get() != 0);
        a.check("03. getNextElement", e->getNextElement(entry));

        // Delete an element
        AFL_CHECK_SUCCEEDS(a("04. erase"), md->erase(names[toDelete]));

        // Move the iterator to the end. Must not crash.
        do {
            a.check("05. next", entry.get() != 0);
        } while (e->getNextElement(entry));
    }
}

/** Test move between different instances. */
AFL_TEST("afl.io.InternalDirectory:move:by-copy", a)
{
    Ref<Directory> aa = InternalDirectory::create("aa");
    Ref<Directory> bb = InternalDirectory::create("bb");
    aa->openFile("test", FileSystem::Create)->fullWrite(afl::string::toBytes("abc"));
    bb->openFile("other", FileSystem::Create)->fullWrite(afl::string::toBytes("x"));

    // Direct move fails
    AFL_CHECK_THROWS(a("01. moveTo"), aa->getDirectoryEntryByName("test")->moveTo(*bb, "other"), FileProblemException);
    a.checkEqual("02. moveToNT", aa->getDirectoryEntryByName("test")->moveToNT(*bb, "other"), false);
    a.checkEqual("03. aa size", aa->openFile("test", FileSystem::OpenRead)->getSize(), 3U);
    a.checkEqual("04. bb size", bb->openFile("other", FileSystem::OpenRead)->getSize(), 1U);

    // Move by copy succeeds
    AFL_CHECK_SUCCEEDS(a("11. moveFileByCopying"), aa->getDirectoryEntryByName("test")->moveFileByCopying(*bb, "other"));
    AFL_CHECK_THROWS(a("12. aa size"), aa->openFile("test", FileSystem::OpenRead), FileProblemException);
    a.checkEqual("13. bb size", bb->openFile("other", FileSystem::OpenRead)->getSize(), 3U);
}

/** Test move within same instance. */
AFL_TEST("afl.io.InternalDirectory:move:as-rename", a)
{
    Ref<Directory> aa = InternalDirectory::create("aa");
    aa->openFile("test", FileSystem::Create)->fullWrite(afl::string::toBytes("abc"));

    // Direct move fails
    aa->getDirectoryEntryByName("test")->moveTo(*aa, "new");
    a.checkEqual("01. size", aa->openFile("new", FileSystem::OpenRead)->getSize(), 3U);
}

/** Test move within same instance, no name change. */
AFL_TEST("afl.io.InternalDirectory:move:no-op", a)
{
    Ref<Directory> aa = InternalDirectory::create("aa");
    aa->openFile("test", FileSystem::Create)->fullWrite(afl::string::toBytes("abc"));

    // Direct move fails
    aa->getDirectoryEntryByName("test")->moveTo(*aa, "test");
    a.checkEqual("01. size", aa->openFile("test", FileSystem::OpenRead)->getSize(), 3U);
}
