/**
  *  \file afl/io/nullfilesystem.cpp
  *  \brief Class afl::io::NullFileSystem
  */

#include "afl/io/nullfilesystem.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/string/messages.hpp"

// Constructor.
afl::io::NullFileSystem::NullFileSystem()
    : m_fileNames()
{ }

// Destructor.
afl::io::NullFileSystem::~NullFileSystem()
{ }

// Open a file given its name.
afl::base::Ptr<afl::io::Stream>
afl::io::NullFileSystem::openFile(FileName_t fileName, OpenMode /*mode*/)
{
    throw afl::except::FileProblemException(fileName, afl::string::Messages::fileNotFound());
}

// Open a directory given its name.
afl::base::Ptr<afl::io::Directory>
afl::io::NullFileSystem::openDirectory(FileName_t dirName)
{
    throw afl::except::FileProblemException(dirName, afl::string::Messages::fileNotFound());
}

// Open virtual root directory.
afl::base::Ptr<afl::io::Directory>
afl::io::NullFileSystem::openRootDirectory()
{
    throw afl::except::FileProblemException("/", afl::string::Messages::fileNotFound());
}

// Check for absolute path name.
bool
afl::io::NullFileSystem::isAbsolutePathName(FileName_t path)
{
    return m_fileNames.isAbsolutePathName(path);
}

// Check for path separator character.
bool
afl::io::NullFileSystem::isPathSeparator(char c)
{
    return m_fileNames.isPathSeparator(c);
}

// Make path name from path and subsequent file name.
afl::io::NullFileSystem::FileName_t
afl::io::NullFileSystem::makePathName(FileName_t path, FileName_t name)
{
    return m_fileNames.makePathName(path, name);
}

// Get canonical path.
afl::io::NullFileSystem::FileName_t
afl::io::NullFileSystem::getCanonicalPathName(FileName_t name)
{
    return m_fileNames.getCanonicalPathName(name);
}

// Get absolute path.
afl::io::NullFileSystem::FileName_t
afl::io::NullFileSystem::getAbsolutePathName(FileName_t name)
{
    if (isAbsolutePathName(name)) {
        return getCanonicalPathName(name);
    } else {
        return getCanonicalPathName(makePathName(getWorkingDirectoryName(), name));
    }
}

// Get file name.
afl::io::NullFileSystem::FileName_t
afl::io::NullFileSystem::getFileName(FileName_t name)
{
    return m_fileNames.getFileName(name);
}

// Get directory name.
afl::io::NullFileSystem::FileName_t
afl::io::NullFileSystem::getDirectoryName(FileName_t name)
{
    return m_fileNames.getDirectoryName(name);
}

// Get name of current working directory.
afl::io::NullFileSystem::FileName_t
afl::io::NullFileSystem::getWorkingDirectoryName()
{
    return "/";
}
