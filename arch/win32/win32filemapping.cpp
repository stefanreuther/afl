/**
  *  \file arch/win32/win32filemapping.cpp
  *  \brief Class arch::win32::Win32FileMapping
  */

#if TARGET_OS_WIN32
#include <algorithm>
#include "arch/win32/win32filemapping.hpp"

namespace {
    size_t getPageSize()
    {
	static size_t result = 0;
	if (result == 0) {
	    SYSTEM_INFO info;
	    GetSystemInfo(&info);
	    if (info.dwPageSize <= 0                                     // invalid or unknown
		|| DWORD(size_t(info.dwPageSize)) != info.dwPageSize     // cannot be represented
		|| (info.dwPageSize & (info.dwPageSize-1)) != 0)         // not a power of two
	    {
		result = 4096;
	    } else {
		result = size_t(info.dwPageSize);
	    }
	}
	return result;
    }
}

// Constructor.
arch::win32::Win32FileMapping::Win32FileMapping(void* address, size_t bytesBefore, size_t bytesMapped)
    : m_address(address),
      m_bytesBefore(bytesBefore),
      m_bytesMapped(bytesMapped)
{ }

// Destructor.
arch::win32::Win32FileMapping::~Win32FileMapping()
{
    UnmapViewOfFile(m_address);
}

// Get content of file mapping.
afl::base::ConstBytes_t
arch::win32::Win32FileMapping::get() const
{
    return afl::base::ConstBytes_t::unsafeCreate(static_cast<uint8_t*>(m_address), m_bytesMapped).subrange(m_bytesBefore);
}

// Create file mapping.
afl::base::Ptr<afl::io::FileMapping>
arch::win32::Win32FileMapping::createFileMapping(HANDLE hFile, afl::io::Stream::FileSize_t limit)
{
    typedef afl::io::Stream::FileSize_t FileSize_t;

    // Determine file size
    DWORD highHalf = 0;
    DWORD lowHalf = GetFileSize(hFile, &highHalf);
    if (lowHalf == 0xFFFFFFFFU && GetLastError() != NO_ERROR) {
        return 0;
    }
    FileSize_t fileSize = (FileSize_t(highHalf) << 32) + lowHalf;

    // Determine file position
    LONG lowHalfLong = 0;
    LONG highHalfLong = 0;
    DWORD lowHalfResult = SetFilePointer(hFile, lowHalfLong, &highHalfLong, FILE_CURRENT);
    if (lowHalfResult == 0xFFFFFFFFU && GetLastError() != NO_ERROR) {
        return 0;
    }
    FileSize_t filePosition = (FileSize_t(highHalfLong) << 32) + DWORD(lowHalfResult);

    // If position is after size of file, the mapped size will be zero.
    // For simplicity, fall back to the default.
    if (filePosition >= fileSize) {
	return 0;
    }

    // Compute number of bytes to map. Refuse to map more than address-space minus one page.
    size_t pageSize = getPageSize();
    afl::io::Stream::FileSize_t bytesToMap = std::min(limit, afl::io::Stream::FileSize_t(fileSize - filePosition));
    size_t sizeToMap = size_t(bytesToMap);
    if (sizeToMap != bytesToMap) {
	return 0;
    }
    if (sizeToMap + pageSize < sizeToMap) {
	return 0;
    }

    // Compute starting address. Must be a multiple of the page size
    size_t bytesBefore = size_t(filePosition & (pageSize-1));
    filePosition -= bytesBefore;
    sizeToMap += bytesBefore;

    // Create a file mapping object
    HANDLE hMap = CreateFileMapping(hFile, 0, PAGE_READONLY, DWORD(uint64_t(sizeToMap) >> 32), DWORD(sizeToMap), 0);
    if (hMap == 0) {
	return 0;
    }

    // Create a mapping
    void* address = MapViewOfFile(hMap, FILE_MAP_READ, DWORD(filePosition >> 32), DWORD(filePosition), sizeToMap);
    if (hMap == 0) {
	CloseHandle(hMap);
	return 0;
    }

    // At this point, we can close the file mapping handle.
    // MSDN says UnmapViewOfFile and CloseHandle can be called in any order.
    // Closing the file mapping here means one less thing to deal with later.
    CloseHandle(hMap);

    // Advance file pointer
    lowHalfLong = LONG(filePosition + sizeToMap);
    highHalfLong = LONG((filePosition + sizeToMap) >> 32);
    SetFilePointer(hFile, lowHalfLong, &highHalfLong, FILE_BEGIN);

    // Success! Build a handle for it. Be careful that constructing the handle might fail.
    afl::base::Ptr<afl::io::FileMapping> result;
    try {
	result = new Win32FileMapping(address, bytesBefore, sizeToMap);
    }
    catch (...) {
	UnmapViewOfFile(address);
	throw;
    }
    return result;
}

#else
int g_variableToMakeWin32FileMappingObjectFileNotEmpty;
#endif
