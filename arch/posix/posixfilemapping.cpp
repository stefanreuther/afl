/**
  *  \file arch/posix/posixfilemapping.cpp
  *  \brief Class arch::posix::PosixFileMapping
  */

#include <algorithm>
#if TARGET_OS_POSIX
#define _FILE_OFFSET_BITS 64
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include "arch/posix/posixfilemapping.hpp"
#include "arch/posix/posixstream.hpp"

namespace {
    size_t getPageSize()
    {
        static size_t result = 0;
        if (result == 0) {
            long x = sysconf(_SC_PAGESIZE);
            if (x <= 0                         // value is reported as invalid or unknown
                || long(size_t(x)) != x        // value is invalid because it cannot be represented
                || (x & (x-1)) != 0)           // value is invalid because it's not a power of 2
            {
                result = 4096;
            } else {
                result = size_t(x);
            }
        }
        return result;
    }
}

// Constructor.
arch::posix::PosixFileMapping::PosixFileMapping(void* address, size_t bytesBefore, size_t bytesMapped)
    : m_address(address),
      m_bytesBefore(bytesBefore),
      m_bytesMapped(bytesMapped)
{ }

// Destructor.
arch::posix::PosixFileMapping::~PosixFileMapping()
{
    ::munmap(m_address, m_bytesMapped);
}

// Get content of file mapping.
afl::base::ConstBytes_t
arch::posix::PosixFileMapping::get() const
{
    return afl::base::ConstBytes_t::unsafeCreate(static_cast<uint8_t*>(m_address), m_bytesMapped).subrange(m_bytesBefore);
}

// Create file mapping.
afl::base::Ptr<afl::io::FileMapping>
arch::posix::PosixFileMapping::createFileMapping(int fd, afl::io::Stream::FileSize_t limit)
{
    // Must behave like a real file
    off_t pos = ::lseek(fd, 0, SEEK_CUR);
    if (pos == off_t(-1)) {
        return 0;
    }
    struct stat st;
    if (::fstat(fd, &st) != 0) {
        return 0;
    }

    // If position is after size of file, the mapped size will be zero.
    // For simplicity, fall back to the default.
    if (pos >= st.st_size) {
        return 0;
    }

    // Compute number of bytes to map. Refuse to map more than address-space minus one page.
    size_t pageSize = getPageSize();
    afl::io::Stream::FileSize_t bytesToMap = std::min(limit, afl::io::Stream::FileSize_t(st.st_size - pos));
    size_t sizeToMap = size_t(bytesToMap);
    if (sizeToMap != bytesToMap) {
        return 0;
    }
    if (sizeToMap + pageSize < sizeToMap) {
        return 0;
    }

    // Compute starting address. Must be a multiple of the page size
    size_t bytesBefore = size_t(pos) & (pageSize-1);
    pos -= bytesBefore;
    sizeToMap += bytesBefore;

    // OK, try to map
    void* address = ::mmap(0, sizeToMap, PROT_READ, MAP_SHARED, fd, pos);
    if (address == MAP_FAILED) {
        return 0;
    }
    ::lseek(fd, pos + off_t(sizeToMap), SEEK_SET);

    // Success! Build a handle for it. Be careful that constructing the handle might fail.
    afl::base::Ptr<afl::io::FileMapping> result;
    try {
        result = new PosixFileMapping(address, bytesBefore, sizeToMap);
    }
    catch (...) {
        ::munmap(address, sizeToMap);
        throw;
    }
    return result;
}

#else
int g_variableToMakePosixFileMappingObjectFileNotEmpty;
#endif
