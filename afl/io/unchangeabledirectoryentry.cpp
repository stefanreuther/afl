/**
  *  \file afl/io/unchangeabledirectoryentry.cpp
  *  \brief afl::io::UnchangeableDirectoryEntry
  */

#include "afl/io/unchangeabledirectoryentry.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/string/messages.hpp"

afl::base::Ref<afl::io::Stream>
afl::io::UnchangeableDirectoryEntry::openFile(FileSystem::OpenMode mode)
{
    if (mode != FileSystem::OpenRead) {
        fail();
    }
    return openFileForReading();
}

void
afl::io::UnchangeableDirectoryEntry::doRename(String_t /*newName*/)
{
    fail();
}

void
afl::io::UnchangeableDirectoryEntry::doErase()
{
    fail();
}

void
afl::io::UnchangeableDirectoryEntry::doCreateAsDirectory()
{
    fail();
}

void
afl::io::UnchangeableDirectoryEntry::doSetFlag(FileFlag /*flag*/, bool /*value*/)
{
    fail();
}

void
afl::io::UnchangeableDirectoryEntry::doMoveTo(Directory& /*dir*/, String_t /*name*/)
{
    fail();
}

void
afl::io::UnchangeableDirectoryEntry::fail()
{
    String_t name = getPathName();
    if (name.empty()) {
        name = getTitle();
    }
    throw afl::except::FileProblemException(name, m_errorMessage);
}
