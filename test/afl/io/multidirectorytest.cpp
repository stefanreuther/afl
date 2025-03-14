/**
  *  \file test/afl/io/multidirectorytest.cpp
  *  \brief Test for afl::io::MultiDirectory
  */

#include "afl/io/multidirectory.hpp"

#include "afl/base/ptr.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/io/directoryentry.hpp"
#include "afl/io/internaldirectory.hpp"
#include "afl/test/testrunner.hpp"

using afl::base::Ptr;
using afl::base::Ref;
using afl::io::DirectoryEntry;
using afl::io::InternalDirectory;
using afl::io::MultiDirectory;

/** Test (in)ability to create files. */
AFL_TEST("afl.io.MultiDirectory:create-files", a)
{
    Ref<MultiDirectory> md(MultiDirectory::create());
    Ref<InternalDirectory> id(InternalDirectory::create("a"));
    md->addDirectory(id);

    // Creating files in the InternalDirectory works
    {
        Ref<afl::io::Stream> s = id->openFile("c", afl::io::FileSystem::Create);
        a.checkNonNull("internal create", s.asPtr().get());
        s->write(afl::string::toBytes("foo"));
        a.checkEqual("internal size", s->getSize(), 3U);
    }

    // Creating files in the MultiDirectory does not work
    AFL_CHECK_THROWS(a("multi create a"), md->openFile("a", afl::io::FileSystem::Create), afl::except::FileProblemException);
    AFL_CHECK_THROWS(a("multi create b"), md->openFile("b", afl::io::FileSystem::Create), afl::except::FileProblemException);

    // Recreating an existing file works and resets that to size 0 (InternalDirectory feature)
    {
        Ref<afl::io::Stream> s = id->openFile("c", afl::io::FileSystem::Create);
        a.checkNonNull("recreate", s.asPtr().get());
        a.checkEqual("recreate size", s->getSize(), 0U);
    }
}

/** Test name lookup. */
AFL_TEST("afl.io.MultiDirectory:lookup", a)
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
    a.checkNonNull("openFile a", md->openFile("a", afl::io::FileSystem::OpenRead).asPtr().get());
    a.checkNonNull("openFile b", md->openFile("b", afl::io::FileSystem::OpenRead).asPtr().get());
    a.checkNonNull("openFile c", md->openFile("c", afl::io::FileSystem::OpenRead).asPtr().get());

    // Must be able to look up all of them
    a.checkNonNull("getDirectoryEntryByName a", md->getDirectoryEntryByName("a").asPtr().get());
    a.checkNonNull("getDirectoryEntryByName b", md->getDirectoryEntryByName("b").asPtr().get());
    a.checkNonNull("getDirectoryEntryByName c", md->getDirectoryEntryByName("c").asPtr().get());

    // The common file must come from id1
    a.check("file from directory b", md->getDirectoryEntryByName("b")->openContainingDirectory().asPtr() == id1.asPtr());
}

/** Test member enumeration. */
AFL_TEST("afl.io.MultiDirectory:getDirectoryEntries", a)
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
    a.checkNonNull("enumerator is non-null", &e.get());
    bool seenA = false;
    bool seenB = false;
    bool seenC = false;
    Ptr<DirectoryEntry> entry;
    while (e->getNextElement(entry)) {
        a.checkNonNull("DirectoryEntry is non-null", entry.get());
        if (entry->getTitle() == "a") {
            a.check("not seen a before", !seenA);
            seenA = true;
        } else if (entry->getTitle() == "b") {
            a.check("not seen b before", !seenB);
            seenB = true;
        } else if (entry->getTitle() == "c") {
            a.check("not seen c before", !seenC);
            seenC = true;
        } else {
            a.fail("seen unexpected file");
        }
    }
    a.check("seen a", seenA);
    a.check("seen b", seenB);
    a.check("seen c", seenC);
}

/** Test behaviour of empty MultiDirectory. */
AFL_TEST("afl.io.MultiDirectory:empty", a)
{
    Ref<MultiDirectory> md(MultiDirectory::create());

    // We cannot create
    AFL_CHECK_THROWS(a("cannot create"), md->openFile("a", afl::io::FileSystem::Create), afl::except::FileProblemException);

    // We cannot open
    AFL_CHECK_THROWS(a("cannot open"), md->openFile("a", afl::io::FileSystem::OpenRead), afl::except::FileProblemException);

    // We can enumerate but don't get anything
    Ref<afl::base::Enumerator<Ptr<DirectoryEntry> > > e = md->getDirectoryEntries();
    a.checkNonNull("enumerator is non-null", &e.get());
    Ptr<DirectoryEntry> entry;
    a.check("no content", !e->getNextElement(entry));
}
