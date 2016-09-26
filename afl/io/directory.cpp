/**
  *  \file afl/io/directory.cpp
  *  \brief Base Class afl::io::Directory
  */

#include "afl/io/directory.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/io/directoryentry.hpp"

afl::io::Directory::~Directory()
{ }

afl::base::Ptr<afl::io::Directory>
afl::io::Directory::openDirectory(String_t name)
{
    return getDirectoryEntryByName(name)->openDirectory();
}

afl::base::Ptr<afl::io::Stream>
afl::io::Directory::openFile(String_t name, FileSystem::OpenMode mode)
{
    return getDirectoryEntryByName(name)->openFile(mode);
}

afl::base::Ptr<afl::io::Stream>
afl::io::Directory::openFileNT(String_t name, FileSystem::OpenMode mode)
{
    try {
        return openFile(name, mode);
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
