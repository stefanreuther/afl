/**
  *  \file arch/win32/win32stream.cpp
  *  \brief Class arch::win32::Win32Stream
  */

#if TARGET_OS_WIN32
#include "arch/win32/win32stream.hpp"
#include "afl/except/filesystemexception.hpp"
#include "afl/sys/error.hpp"
#include "afl/string/messages.hpp"
#include "arch/win32/win32.hpp"
#include "arch/win32/win32filemapping.hpp"

arch::win32::Win32Stream::Win32Stream(afl::io::FileSystem::FileName_t name, afl::io::FileSystem::OpenMode mode)
    : afl::io::MultiplexableStream(),
      afl::base::Uncopyable(),
      m_handle(INVALID_HANDLE_VALUE),
      m_name(name),
      m_capabilities(0)
{
    init(name, mode);
}

arch::win32::Win32Stream::Win32Stream(afl::io::FileSystem::FileName_t name, HANDLE h)
    : afl::io::MultiplexableStream(),
      afl::base::Uncopyable(),
      m_handle(INVALID_HANDLE_VALUE),
      m_name(name),
      m_capabilities(0)
{
    init(h);
}

arch::win32::Win32Stream::~Win32Stream()
{
    if (m_handle != INVALID_HANDLE_VALUE) {
        ::CloseHandle(m_handle);
    }
}

size_t
arch::win32::Win32Stream::read(Bytes_t m)
{
    DWORD got = 0;
    if (!ReadFile(m_handle, m.unsafeData(), convertSizeToDWORD(m.size()), &got, 0)) {
        error();
    }
    return got;
}

size_t
arch::win32::Win32Stream::write(ConstBytes_t m)
{
    DWORD written = 0;
    if (!WriteFile(m_handle, m.unsafeData(), convertSizeToDWORD(m.size()), &written, 0)) {
        error();
    }
    return written;
}

void
arch::win32::Win32Stream::flush()
{ }

void
arch::win32::Win32Stream::setPos(FileSize_t pos)
{
    LONG lowHalf = LONG(pos);
    LONG highHalf = LONG(pos >> 32);
    DWORD result = SetFilePointer(m_handle, lowHalf, &highHalf, FILE_BEGIN);
    if (result == 0xFFFFFFFFU && GetLastError() != NO_ERROR) {
        error();
    }
}

arch::win32::Win32Stream::FileSize_t
arch::win32::Win32Stream::getPos()
{
    LONG lowHalf = 0;
    LONG highHalf = 0;
    DWORD result = SetFilePointer(m_handle, lowHalf, &highHalf, FILE_CURRENT);
    if (result == 0xFFFFFFFFU && GetLastError() != NO_ERROR) {
        return FileSize_t(-1);
    } else {
        return (FileSize_t(highHalf) << 32) + DWORD(result);
    }
}

arch::win32::Win32Stream::FileSize_t
arch::win32::Win32Stream::getSize()
{
    DWORD highHalf = 0;
    DWORD lowHalf = GetFileSize(m_handle, &highHalf);
    if (lowHalf == 0xFFFFFFFFU && GetLastError() != NO_ERROR) {
        return FileSize_t(-1);
    } else {
        return (FileSize_t(highHalf) << 32) + lowHalf;
    }
}

uint32_t
arch::win32::Win32Stream::getCapabilities()
{
    return m_capabilities;
}

String_t
arch::win32::Win32Stream::getName()
{
    return m_name;
}

afl::base::Ptr<afl::io::FileMapping>
arch::win32::Win32Stream::createFileMapping(FileSize_t limit)
{
    return Win32FileMapping::createFileMapping(m_handle, limit);
}

void
arch::win32::Win32Stream::init(afl::io::FileSystem::FileName_t name, afl::io::FileSystem::OpenMode mode)
{
    DWORD desiredAccess = 0;
    DWORD creationDistribution = 0;
    DWORD shareMode = 0;

    switch (mode) {
     case afl::io::FileSystem::OpenRead:
        m_capabilities = CanRead | CanSeek;
        desiredAccess = GENERIC_READ;
        creationDistribution = OPEN_EXISTING;
        shareMode = FILE_SHARE_READ;
        break;

     case afl::io::FileSystem::OpenWrite:
        m_capabilities = CanRead | CanWrite | CanSeek;
        desiredAccess = GENERIC_READ | GENERIC_WRITE;
        creationDistribution = OPEN_EXISTING;
        shareMode = 0;
        break;

     case afl::io::FileSystem::Create:
        m_capabilities = CanRead | CanWrite | CanSeek;
        desiredAccess = GENERIC_READ | GENERIC_WRITE;
        creationDistribution = CREATE_ALWAYS;
        shareMode = 0;
        break;

     case afl::io::FileSystem::CreateNew:
        m_capabilities = CanRead | CanWrite | CanSeek;
        desiredAccess = GENERIC_READ | GENERIC_WRITE;
        creationDistribution = CREATE_NEW;
        shareMode = 0;
        break;
    }

    if (desiredAccess == 0) {
        // Invalid mode
        throw afl::except::FileProblemException(name, afl::string::Messages::invalidOperation());
    }

    // Perform the syscall
    if (hasUnicodeSupport()) {
        WStr uniName;
        convertToUnicode(uniName, afl::string::toMemory(name));
        if (!terminateUnicode(uniName)) {
            throw afl::except::FileProblemException(name, afl::string::Messages::invalidFileName());
        }
        m_handle = ::CreateFileW(&uniName[0], desiredAccess, shareMode, 0, creationDistribution, FILE_ATTRIBUTE_NORMAL, 0);
    } else {
        String_t ansiName = convertToANSI(afl::string::toMemory(name));
        if (ansiName.find('\0') != String_t::npos) {
            throw afl::except::FileProblemException(name, afl::string::Messages::invalidFileName());
        }
        m_handle = ::CreateFileA(ansiName.c_str(), desiredAccess, shareMode, 0, creationDistribution, FILE_ATTRIBUTE_NORMAL, 0);
    }

    // Check result
    if (m_handle == 0) {
        // Win95 sometimes reports NULL, not INVALID_HANDLE_VALUE
        m_handle = INVALID_HANDLE_VALUE;
    }
    if (m_handle == INVALID_HANDLE_VALUE) {
        error();
    }
}

void
arch::win32::Win32Stream::init(HANDLE h)
{
    m_handle = h;

    // There doesn't seem to be a way to query whether this handle can be read or written, so assume positive thinking.
    m_capabilities = CanRead | CanWrite;

    // Determine whether we can seek by trying it
    LONG lowHalf = 0;
    LONG highHalf = 0;
    DWORD result = SetFilePointer(m_handle, lowHalf, &highHalf, FILE_CURRENT);
    if (result != 0xFFFFFFFFU || GetLastError() == NO_ERROR) {
        m_capabilities |= CanSeek;
    }
}

void
arch::win32::Win32Stream::error()
{
    throw afl::except::FileSystemException(*this, afl::sys::Error::current());
}
#else
int g_variableToMakeWin32StreamObjectFileNotEmpty;
#endif
