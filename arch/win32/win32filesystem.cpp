/**
  *  \file arch/win32/win32filesystem.cpp
  *  \brief Class arch::win32::Win32FileSystem
  */

#if TARGET_OS_WIN32
#define NOMINMAX
#include <list>
#include <windows.h>
#include "arch/win32/win32filesystem.hpp"
#include "arch/win32/win32stream.hpp"
#include "arch/win32/win32directory.hpp"
#include "arch/win32/win32root.hpp"
#include "arch/win32/win32.hpp"

afl::base::Ref<afl::io::Stream>
arch::win32::Win32FileSystem::openFile(FileName_t fileName, OpenMode mode)
{
    return *new Win32Stream(fileName, mode);
}

afl::base::Ref<afl::io::Directory>
arch::win32::Win32FileSystem::openDirectory(FileName_t dirName)
{
    return *new Win32Directory(dirName);
}

afl::base::Ref<afl::io::Directory>
arch::win32::Win32FileSystem::openRootDirectory()
{
    return *new Win32Root();
}

bool
arch::win32::Win32FileSystem::isAbsolutePathName(FileName_t path)
{
    return m_fileNames.isAbsolutePathName(path);
}

bool
arch::win32::Win32FileSystem::isPathSeparator(char c)
{
    return m_fileNames.isPathSeparator(c);
}

arch::win32::Win32FileSystem::FileName_t
arch::win32::Win32FileSystem::makePathName(FileName_t path, FileName_t name)
{
    return m_fileNames.makePathName(path, name);
}

arch::win32::Win32FileSystem::FileName_t
arch::win32::Win32FileSystem::getCanonicalPathName(FileName_t name)
{
    return m_fileNames.getCanonicalPathName(name);
}

arch::win32::Win32FileSystem::FileName_t
arch::win32::Win32FileSystem::getAbsolutePathName(FileName_t name)
{
    // Convert file name
    m_fileNames.flipSlashes(name);

    if (hasUnicodeSupport()) {
        // Unicode version
        WStr tmp;
        convertToUnicode(tmp, afl::string::toMemory(name));
        if (!terminateUnicode(tmp)) {
            // Failure
            return getCanonicalPathName(name);
        } else {
            wchar_t* tmpp;
            wchar_t tmpc;
            int charsNeeded = GetFullPathNameW(&tmp[0], 0, &tmpc, &tmpp);
            if (charsNeeded < 2) {
                // Failure, do the best we can do
                return getCanonicalPathName(name);
            } else {
                // okay, use Win32 function to do it:
                WStr buffer(charsNeeded+1);
                int charsProduced = GetFullPathNameW(&tmp[0], charsNeeded+1, &buffer.front(), &tmpp);
                return convertFromUnicode(afl::base::Memory<const wchar_t>::unsafeCreate(&buffer[0], charsProduced));
            }
        }
    } else {
        // ANSI version
        FileName_t tmp = convertToANSI(afl::string::toMemory(name));

        char* tmpp;
        char tmpc;
        int charsNeeded = GetFullPathNameA(tmp.c_str(), 0, &tmpc, &tmpp);
        if (charsNeeded < 2) {
            // Failure, do the best we can do
            return getCanonicalPathName(name);
        } else {
            // okay, use Win32 function to do it:
            std::vector<char> buffer(charsNeeded+1);
            int charsProduced = GetFullPathNameA(tmp.c_str(), charsNeeded+1, &buffer.front(), &tmpp);
            return convertFromANSI(afl::base::Memory<const char>::unsafeCreate(&buffer[0], charsProduced));
        }
    }
}

arch::win32::Win32FileSystem::FileName_t
arch::win32::Win32FileSystem::getFileName(FileName_t name)
{
    return m_fileNames.getFileName(name);
}

arch::win32::Win32FileSystem::FileName_t
arch::win32::Win32FileSystem::getDirectoryName(FileName_t name)
{
    return m_fileNames.getDirectoryName(name);
}

arch::win32::Win32FileSystem::FileName_t
arch::win32::Win32FileSystem::getWorkingDirectoryName()
{
#ifndef PATH_MAX
    static const std::size_t PATH_MAX = 256;
#endif

    if (hasUnicodeSupport()) {
        std::vector<wchar_t> buffer(PATH_MAX);
        DWORD size;
        while ((size = GetCurrentDirectoryW(convertSizeToDWORD(buffer.size()), &buffer[0])) >= buffer.size()) {
            buffer.resize(buffer.size() + PATH_MAX);
        }
        return convertFromUnicode(afl::base::Memory<const wchar_t>::unsafeCreate(&buffer[0], size));
    } else {
        std::vector<char> buffer(PATH_MAX);
        DWORD size;
        while ((size = GetCurrentDirectoryA(convertSizeToDWORD(buffer.size()), &buffer[0])) >= buffer.size()) {
            buffer.resize(buffer.size() + PATH_MAX);
        }
        return convertFromANSI(afl::base::Memory<char>::unsafeCreate(&buffer[0], size));
    }
}

#else
int g_variableToMakeWin32FileSystemObjectFileNotEmpty;
#endif
