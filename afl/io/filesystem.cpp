/**
  *  \file afl/io/filesystem.cpp
  *  \brief Class afl::io::FileSystem
  */

#include "afl/io/filesystem.hpp"
#include "arch/filesystem.hpp"
#include "afl/except/fileproblemexception.hpp"

afl::io::FileSystem&
afl::io::FileSystem::getInstance()
{
    static FileSystem_t instance;
    return instance;
}

// Open a file, but don't throw FileProblemException's.
afl::base::Ptr<afl::io::Stream>
afl::io::FileSystem::openFileNT(String_t name, OpenMode mode)
{
    try {
        return openFile(name, mode).asPtr();
    }
    catch (afl::except::FileProblemException&) {
        return 0;
    }
}
