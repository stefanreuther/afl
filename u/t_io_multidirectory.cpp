/**
  *  \file u/t_io_multidirectory.cpp
  *  \brief Test for afl::io::MultiDirectory
  */

#include "afl/io/multidirectory.hpp"

#include "u/t_io.hpp"
#include "afl/base/ptr.hpp"
#include "afl/io/internaldirectory.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/io/directoryentry.hpp"

using afl::base::Ptr;
using afl::base::Ref;
using afl::io::MultiDirectory;
using afl::io::DirectoryEntry;
using afl::io::InternalDirectory;

/** Test (in)ability to create files. */
void
TestIoMultiDirectory::testCreate()
{
    Ref<MultiDirectory> md(MultiDirectory::create());
    Ref<InternalDirectory> id(InternalDirectory::create("a"));
    md->addDirectory(id);

    // Creating files in the InternalDirectory works
    {
        Ref<afl::io::Stream> s = id->openFile("c", afl::io::FileSystem::Create);
        TS_ASSERT(&s.get());
        s->write(afl::string::toBytes("foo"));
        TS_ASSERT_EQUALS(s->getSize(), 3U);
    }

    // Creating files in the MultiDirectory does not work
    TS_ASSERT_THROWS(md->openFile("a", afl::io::FileSystem::Create), afl::except::FileProblemException);
    TS_ASSERT_THROWS(md->openFile("b", afl::io::FileSystem::Create), afl::except::FileProblemException);

    // Recreating an existing file works and resets that to size 0 (InternalDirectory feature)
    {
        Ref<afl::io::Stream> s = id->openFile("c", afl::io::FileSystem::Create);
        TS_ASSERT(&s.get());
        TS_ASSERT_EQUALS(s->getSize(), 0U);
    }
}

/** Test name lookup. */
void
TestIoMultiDirectory::testLookup()
{
    // Populate with multiple files
    Ref<MultiDirectory> md(MultiDirectory::create());
    Ref<InternalDirectory> id1(InternalDirectory::create("a"));
    md->addDirectory(id1);
    id1->openFile("a", afl::io::FileSystem::Create);
    id1->openFile("b", afl::io::FileSystem::Create);

    Ref<InternalDirectory> id2(InternalDirectory::create("a"));
    md->addDirectory(id2);
    id2->openFile("b", afl::io::FileSystem::Create);
    id2->openFile("c", afl::io::FileSystem::Create);

    // Must be able to open all of them
    TS_ASSERT(&md->openFile("a", afl::io::FileSystem::OpenRead).get());
    TS_ASSERT(&md->openFile("b", afl::io::FileSystem::OpenRead).get());
    TS_ASSERT(&md->openFile("c", afl::io::FileSystem::OpenRead).get());

    // Must be able to look up all of them
    TS_ASSERT(&md->getDirectoryEntryByName("a").get());
    TS_ASSERT(&md->getDirectoryEntryByName("b").get());
    TS_ASSERT(&md->getDirectoryEntryByName("c").get());

    // The common file must come from id1
    TS_ASSERT(md->getDirectoryEntryByName("b")->openContainingDirectory().asPtr() == id1.asPtr());
}

/** Test member enumeration. */
void
TestIoMultiDirectory::testEnum()
{
    // Populate with multiple files
    Ref<MultiDirectory> md(MultiDirectory::create());
    Ref<InternalDirectory> id1(InternalDirectory::create("a"));
    md->addDirectory(id1);
    id1->openFile("a", afl::io::FileSystem::Create);
    id1->openFile("b", afl::io::FileSystem::Create);

    Ref<InternalDirectory> id2(InternalDirectory::create("a"));
    md->addDirectory(id2);
    id2->openFile("b", afl::io::FileSystem::Create);
    id2->openFile("c", afl::io::FileSystem::Create);

    // Enumerate
    Ref<afl::base::Enumerator<Ptr<DirectoryEntry> > > e = md->getDirectoryEntries();
    TS_ASSERT(&e.get());
    bool seenA = false;
    bool seenB = false;
    bool seenC = false;
    Ptr<DirectoryEntry> entry;
    while (e->getNextElement(entry)) {
        TS_ASSERT(entry.get());
        if (entry->getTitle() == "a") {
            TS_ASSERT(!seenA);
            seenA = true;
        } else if (entry->getTitle() == "b") {
            TS_ASSERT(!seenB);
            seenB = true;
        } else if (entry->getTitle() == "c") {
            TS_ASSERT(!seenC);
            seenC = true;
        } else {
            TS_ASSERT(0);
        }
    }
    TS_ASSERT(seenA);
    TS_ASSERT(seenB);
    TS_ASSERT(seenC);
}

/** Test behaviour of empty MultiDirectory. */
void
TestIoMultiDirectory::testEmpty()
{
    Ref<MultiDirectory> md(MultiDirectory::create());

    // We cannot create
    TS_ASSERT_THROWS(md->openFile("a", afl::io::FileSystem::Create), afl::except::FileProblemException);

    // We cannot open
    TS_ASSERT_THROWS(md->openFile("a", afl::io::FileSystem::OpenRead), afl::except::FileProblemException);

    // We can enumerate but don't get anything
    Ref<afl::base::Enumerator<Ptr<DirectoryEntry> > > e = md->getDirectoryEntries();
    TS_ASSERT(&e.get());
    Ptr<DirectoryEntry> entry;
    TS_ASSERT(!e->getNextElement(entry));
}
