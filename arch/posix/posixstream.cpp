/**
  *  \file arch/posix/posixstream.cpp
  *  \brief Class arch::posix::PosixStream
  */

#if TARGET_OS_POSIX
#define _FILE_OFFSET_BITS 64
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "arch/posix/posixstream.hpp"
#include "afl/except/filesystemexception.hpp"
#include "afl/sys/error.hpp"
#include "arch/posix/posix.hpp"
#include "afl/string/messages.hpp"
#include "arch/posix/posixfilemapping.hpp"

#ifndef O_LARGEFILE
# define O_LARGEFILE 0
#endif

arch::posix::PosixStream::PosixStream(afl::io::FileSystem::FileName_t name, afl::io::FileSystem::OpenMode mode)
    : afl::io::MultiplexableStream(),
      afl::base::Uncopyable(),
      m_fd(-1),
      m_name(name),
      m_capabilities(0)
{
    init(name, mode);
}

arch::posix::PosixStream::PosixStream(afl::io::FileSystem::FileName_t name, int fd)
    : afl::io::MultiplexableStream(),
      afl::base::Uncopyable(),
      m_fd(-1),
      m_name(name),
      m_capabilities(0)
{
    init(fd);
}

arch::posix::PosixStream::~PosixStream()
{
    if (m_fd >= 0) {
        ::close(m_fd);
    }
}

size_t
arch::posix::PosixStream::read(Bytes_t m)
{
    ssize_t n = ::read(m_fd, m.unsafeData(), m.size());
    if (n < 0) {
        error();
    }
    return n;
}

size_t
arch::posix::PosixStream::write(ConstBytes_t m)
{
    ssize_t n = ::write(m_fd, m.unsafeData(), m.size());
    if (n < 0) {
        error();
    }
    return n;
}

void
arch::posix::PosixStream::flush()
{ }

void
arch::posix::PosixStream::setPos(FileSize_t pos)
{
    if (::lseek(m_fd, pos, SEEK_SET) < 0) {
        error();
    }
}

arch::posix::PosixStream::FileSize_t
arch::posix::PosixStream::getPos()
{
    return ::lseek(m_fd, 0, SEEK_CUR);
}

arch::posix::PosixStream::FileSize_t
arch::posix::PosixStream::getSize()
{
    struct stat st;
    if (::fstat(m_fd, &st) == 0) {
        return st.st_size;
    } else {
        return 0;
    }
}

uint32_t
arch::posix::PosixStream::getCapabilities()
{
    return m_capabilities;
}

String_t
arch::posix::PosixStream::getName()
{
    return m_name;
}

afl::base::Ptr<afl::io::FileMapping>
arch::posix::PosixStream::createFileMapping(FileSize_t limit)
{
    return PosixFileMapping::createFileMapping(m_fd, limit);
}

void
arch::posix::PosixStream::init(afl::io::FileSystem::FileName_t name, afl::io::FileSystem::OpenMode mode)
{
    afl::io::FileSystem::FileName_t sysName;
    if (!convertUtf8ToExternal(sysName, afl::string::toMemory(name), false)) {
        throw afl::except::FileProblemException(*this, afl::string::Messages::invalidFileName());
    }

    // Pre-initialize status/errors in case the enum switch is broken somehow
    m_capabilities = 0;
    m_fd = -1;
    errno = EINVAL;

    // Open it
    switch (mode) {
     case afl::io::FileSystem::OpenRead:
        m_capabilities = CanRead | CanSeek;
        m_fd = ::open(sysName.c_str(), O_RDONLY | O_LARGEFILE);
        break;

     case afl::io::FileSystem::OpenWrite:
        m_capabilities = CanRead | CanWrite | CanSeek;
        m_fd = ::open(sysName.c_str(), O_RDWR | O_LARGEFILE);
        break;

     case afl::io::FileSystem::Create:
        m_capabilities = CanRead | CanWrite | CanSeek;
        m_fd = ::open(sysName.c_str(), O_RDWR | O_TRUNC | O_CREAT | O_LARGEFILE, 0666);
        break;
        
     case afl::io::FileSystem::CreateNew:
        m_capabilities = CanRead | CanWrite | CanSeek;
        m_fd = ::open(sysName.c_str(), O_RDWR | O_EXCL | O_CREAT | O_LARGEFILE, 0666);
        break;
    }
 
    if (m_fd < 0) {
        // FIXME: use a FileOpenException?
        error();
    }
}

void
arch::posix::PosixStream::init(int fd)
{
    // Initialize
    m_capabilities = 0;
    m_fd = ::dup(fd);
    if (m_fd < 0) {
        error();
    }

    // Determine capabilities
    int mode = ::fcntl(fd, F_GETFL);
    if (mode < 0) {
        error();
    }
    switch (mode & O_ACCMODE) {
     case O_RDONLY:
        m_capabilities |= CanRead;
        break;

     case O_WRONLY:
        m_capabilities |= CanWrite;
        break;

     case O_RDWR:
        m_capabilities |= CanRead | CanWrite;
        break;

     default:
        break;
    }
    if (::lseek(m_fd, 0, SEEK_CUR) != -1) {
        m_capabilities |= CanSeek;
    }
}

void
arch::posix::PosixStream::error()
{
    throw afl::except::FileSystemException(*this, afl::sys::Error::current());
}
#else
int g_variableToMakePosixStreamObjectFileNotEmpty;
#endif
