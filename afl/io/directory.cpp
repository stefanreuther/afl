/**
  *  \file afl/io/directory.cpp
  *  \brief Base Class afl::io::Directory
  */

#include "afl/io/directory.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/io/directoryentry.hpp"

afl::io::Directory::~Directory()
{ }

afl::base::Ref<afl::io::Directory>
afl::io::Directory::openDirectory(String_t name)
{
    return getDirectoryEntryByName(name)->openDirectory();
}

afl::base::Ref<afl::io::Stream>
afl::io::Directory::openFile(String_t name, FileSystem::OpenMode mode)
{
    return getDirectoryEntryByName(name)->openFile(mode);
}

afl::base::Ptr<afl::io::Stream>
afl::io::Directory::openFileNT(String_t name, FileSystem::OpenMode mode)
{
    try {
        return openFile(name, mode).asPtr();
    }
    catch (afl::except::FileProblemException& e) {
        return 0;
    }
}

void
afl::io::Directory::erase(String_t name)
{
    getDirectoryEntryByName(name)->erase();
}

bool
afl::io::Directory::eraseNT(String_t name)
{
    try {
        erase(name);
        return true;
    }
    catch (afl::except::FileProblemException& e) {
        (void) e;
        return false;
    }
}

void
afl::io::Directory::eraseContentRecursively()
{
    // For now, implement this as a normal recursive procedure.
    // It would make sense to implement it iteratively to avoid stack trouble.

    // First, obtain list of entries and stash them away.
    // (This is to avoid potential unspecified behaviour by modifying a directory being read.)
    std::vector<afl::base::Ptr<DirectoryEntry> > entries;
    afl::base::Ref<afl::base::Enumerator<afl::base::Ptr<DirectoryEntry> > > it = getDirectoryEntries();
    afl::base::Ptr<DirectoryEntry> entry;
    while (it->getNextElement(entry)) {
        if (entry.get() != 0) {
            entries.push_back(entry);
        }
    }

    // Erase recursively
    for (size_t i = 0, n = entries.size(); i < n; ++i) {
        DirectoryEntry& e = *entries[i];
        if (e.getFileType() == DirectoryEntry::tDirectory && !e.getFlags().contains(DirectoryEntry::Link)) {
            e.openDirectory()->eraseContentRecursively();
        }
        e.erase();
    }
}
