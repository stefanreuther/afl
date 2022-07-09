/**
  *  \file afl/io/temporarydirectory.cpp
  *  \brief Class afl::io::TemporaryDirectory
  */

#include "afl/io/temporarydirectory.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/string/format.hpp"
#include "afl/string/messages.hpp"
#include "afl/sys/time.hpp"

namespace {
    // Maximum number of tries to create a directory.
    // Used to detect when we're in a situation where directory creation is impossible.
    const int LIMIT = 1000;
}

// Constructor.
afl::io::TemporaryDirectory::TemporaryDirectory(afl::base::Ref<Directory> parentDirectory)
    : m_entry(create(*parentDirectory))
{ }

// Destructor.
afl::io::TemporaryDirectory::~TemporaryDirectory()
{
    try {
        m_entry->openDirectory()->eraseContentRecursively();
        m_entry->erase();
    }
    catch (afl::except::FileProblemException&) {
        // Sweep under the carpet.
        // Better leak a temporary directory than crashing.
    }
}

// Get directory.
afl::base::Ref<afl::io::Directory>
afl::io::TemporaryDirectory::get()
{
    return m_entry->openDirectory();
}

// Directory creation logic.
afl::base::Ref<afl::io::DirectoryEntry>
afl::io::TemporaryDirectory::create(Directory& parentDirectory)
{
    // A time-based component. Totally arbitrary, but should try to generate unique names quickly.
    const uint64_t time = (afl::sys::Time::getCurrentTime() - afl::sys::Time::fromUnixTime(1600000000)).getMilliseconds();

    // Create
    for (int i = 0; i < LIMIT; ++i) {
        try {
            // Put the loop counter first, so it will have some effect if the file system has file name length limits
            afl::base::Ref<afl::io::DirectoryEntry> e = parentDirectory.getDirectoryEntryByName(afl::string::Format("_tmp%d_%d", i, time));
            e->createAsDirectory();
            return e;
        }
        catch (afl::except::FileProblemException&) {
            // Already exists/not creatible. Try another.
        }
    }

    // If we end up here, we are unable to find a suitable name. Bail out.
    throw afl::except::FileProblemException(parentDirectory.getDirectoryName(), afl::string::Messages::cannotAccessDirectories());
}
