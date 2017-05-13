/**
  *  \file arch/posix/posixfilesystem.cpp
  *  \brief Class arch::posix::PosixFileSystem
  */

#if TARGET_OS_POSIX
#include "arch/posix/posixfilesystem.hpp"
#include "afl/except/systemexception.hpp"
#include "arch/posix/posixcwd.hpp"
#include "arch/posix/posixdirectory.hpp"
#include "arch/posix/posixroot.hpp"
#include "arch/posix/posixstream.hpp"

afl::base::Ref<afl::io::Stream>
arch::posix::PosixFileSystem::openFile(FileName_t fileName, OpenMode mode)
{
    return *new PosixStream(fileName, mode);
}

afl::base::Ref<afl::io::Directory>
arch::posix::PosixFileSystem::openDirectory(FileName_t dirName)
{
    return *new PosixDirectory(dirName);
}

afl::base::Ref<afl::io::Directory>
arch::posix::PosixFileSystem::openRootDirectory()
{
    return *new PosixRoot();
}

bool
arch::posix::PosixFileSystem::isAbsolutePathName(FileName_t path)
{
    return m_fileNames.isAbsolutePathName(path);
}

bool
arch::posix::PosixFileSystem::isPathSeparator(char c)
{
    return m_fileNames.isPathSeparator(c);
}

arch::posix::PosixFileSystem::FileName_t
arch::posix::PosixFileSystem::makePathName(FileName_t path, FileName_t name)
{
    return m_fileNames.makePathName(path, name);
}

arch::posix::PosixFileSystem::FileName_t
arch::posix::PosixFileSystem::getCanonicalPathName(FileName_t name)
{
    return m_fileNames.getCanonicalPathName(name);
}

arch::posix::PosixFileSystem::FileName_t
arch::posix::PosixFileSystem::getAbsolutePathName(FileName_t name)
{
    return arch::posix::getAbsolutePathName(name);
}

arch::posix::PosixFileSystem::FileName_t
arch::posix::PosixFileSystem::getFileName(FileName_t name)
{
    return m_fileNames.getFileName(name);
}

arch::posix::PosixFileSystem::FileName_t
arch::posix::PosixFileSystem::getDirectoryName(FileName_t name)
{
    return m_fileNames.getDirectoryName(name);
}

arch::posix::PosixFileSystem::FileName_t
arch::posix::PosixFileSystem::getWorkingDirectoryName()
{
    return arch::posix::getWorkingDirectoryName();
}

#else
int g_variableToMakePosixFileSystemObjectFileNotEmpty;
#endif
