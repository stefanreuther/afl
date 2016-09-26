/**
  *  \file arch/win32/win32filesystem.cpp
  *  \brief Class arch::win32::Win32FileSystem
  *
  *  \todo UNC support is completely missing here
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

namespace {
    const char PATH_SEPARATOR = '\\';

    /** Convert slashes to backslashes. */
    void
    frobSlash(String_t& name)
    {
        String_t::size_type n = 0;
        while ((n = name.find('/', n)) != name.npos) {
            name[n] = PATH_SEPARATOR;
        }
    }
}

afl::base::Ptr<afl::io::Stream>
arch::win32::Win32FileSystem::openFile(FileName_t fileName, OpenMode mode)
{
    return new Win32Stream(fileName, mode);
}

afl::base::Ptr<afl::io::Directory>
arch::win32::Win32FileSystem::openDirectory(FileName_t dirName)
{
    return new Win32Directory(dirName);
}

afl::base::Ptr<afl::io::Directory>
arch::win32::Win32FileSystem::openRootDirectory()
{
    return new Win32Root();
}

bool
arch::win32::Win32FileSystem::isAbsolutePathName(FileName_t path)
{
    return (path.size() > 2 && path[1] == ':' && (path[2] == '\\' || path[2] == '/'));
}

bool
arch::win32::Win32FileSystem::isPathSeparator(char c)
{
    return c == '\\' || c == ':' || c == '/';
}

arch::win32::Win32FileSystem::FileName_t
arch::win32::Win32FileSystem::makePathName(FileName_t path, FileName_t name)
{
    if (isAbsolutePathName(name) || (name.size() >= 2 && name[1] == ':') || (name.size() > 0 && (name[0] == '/' || name[0] == '\\'))) {
        // absolute ("c:/foo"), drive-relative ("c:foo"), or root-relative ("\foo")
        return name;
    } else {
        // relative
        if (path.size() != 0 && !isPathSeparator(path[path.size()-1])) {
            path += PATH_SEPARATOR;
        }
        return path + name;
    }
}

arch::win32::Win32FileSystem::FileName_t
arch::win32::Win32FileSystem::getCanonicalPathName(FileName_t name)
{
    frobSlash(name);

    // If there is a drive letter, remove it and save it for later. */
    FileName_t prefix;
    if (name.size() >= 1 && name[1] == ':') {
        prefix.assign(name, 0, 2);
        name.erase(0, 2);
    }

    // Convert remainder
    bool isAbsolute = false;
    size_t upCount = 0;
    std::list<String_t> out;

    // Check leading slash
    FileName_t::size_type pos = 0;
    if (name[0] == PATH_SEPARATOR) {
        isAbsolute = true;
        pos = 1;
    }

    while (pos < name.size()) {
        // Find next delimiter
        FileName_t::size_type next = name.find(PATH_SEPARATOR, pos);
        if (next == FileName_t::npos) {
            next = name.size();
        }

        // Process
        if (next == pos) {
            // ignore consecutive slashes
        } else if (next == pos+1 && name[pos] == '.') {
            // ignore "." components
        } else if (next == pos+2 && name.compare(pos, 2, "..", 2) == 0) {
            // ".." component
            if (!out.empty()) {
                out.pop_back();
            } else {
                ++upCount;
            }
        } else {
            // normal
            out.push_back(FileName_t(name, pos, next-pos));
        }

        if (next >= name.size()) {
            break;
        }
        pos = next+1;
    }

    // Now generate output
    FileName_t s;
    if (isAbsolute) {
        s += PATH_SEPARATOR;
    } else {
        while (upCount--) {
            s += "..";
            s += PATH_SEPARATOR;
        }
    }
    for (std::list<FileName_t>::iterator i = out.begin(); i != out.end(); ++i) {
        if (i != out.begin()) {
            s += PATH_SEPARATOR;
        }
        s += *i;
    }

    if (s.empty()) {
        s = ".";
    }
    return prefix + s;
}

arch::win32::Win32FileSystem::FileName_t
arch::win32::Win32FileSystem::getAbsolutePathName(FileName_t name)
{
    // Convert file name
    frobSlash(name);

    if (hasUnicodeSupport()) {
        // Unicode version
        WStr tmp;
        convertToUnicode(tmp, afl::string::toMemory(name));
        tmp.push_back(L'\0');

        wchar_t* tmpp;
        wchar_t tmpc;
        int charsNeeded = GetFullPathNameW(&tmp[0], 0, &tmpc, &tmpp);
        if (charsNeeded < 2) {
            // Failure, do the best we can do
            return getCanonicalPathName(name);
        } else {
            // okay, use Win32 function to do it:
            WStr buffer(charsNeeded+1);
            GetFullPathNameW(&tmp[0], charsNeeded, &buffer.front(), &tmpp);
            return convertFromUnicode(afl::base::Memory<const wchar_t>::unsafeCreate(&buffer[0], buffer.size()-1));
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
            GetFullPathNameA(tmp.c_str(), charsNeeded, &buffer.front(), &tmpp);
            return convertFromANSI(afl::base::Memory<const char>::unsafeCreate(&buffer[0], buffer.size()-1));
        }
    }
}

arch::win32::Win32FileSystem::FileName_t
arch::win32::Win32FileSystem::getFileName(FileName_t name)
{
    FileName_t::size_type d = name.find_last_of("\\:/");
    if (d != name.npos && (name[d] == '\\' || name[d] == '/' || d == 1)) {
        // "fooo\bar", "foo/bar", or "a:" (but not "bla:blub" which is a file with a stream)
        return name.substr(d+1);
    } else {
        // Just a file name
        return name;
    }
}

arch::win32::Win32FileSystem::FileName_t
arch::win32::Win32FileSystem::getDirectoryName(FileName_t name)
{
    frobSlash(name);
    FileName_t::size_type d = name.find_last_of("\\:");
    if (d == 0 && name[d] == '\\') {
        /* `\file' */
        return FileName_t(1, PATH_SEPARATOR);
    } else if (d == 1 && name[1] == ':') {
        /* `a:file' */
        return name.substr(0, 2);
    } else if (d == 2 && name[1] == ':' && name[2] == '\\') {
        /* `a:\file' */
        return name.substr(0, 3);
    } else if (d != FileName_t::npos && name[d] == '\\') {
        /* `dir\file' */
        return name.substr(0, d);
    } else {
        /* `file' */
        return ".";
    }
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
        while ((size = GetCurrentDirectoryW(buffer.size(), &buffer[0])) >= buffer.size()) {
            buffer.resize(buffer.size() + PATH_MAX);
        }
        return convertFromUnicode(afl::base::Memory<const wchar_t>::unsafeCreate(&buffer[0], size));
    } else {
        std::vector<char> buffer(PATH_MAX);
        DWORD size;
        while ((size = GetCurrentDirectoryA(buffer.size(), &buffer[0])) >= buffer.size()) {
            buffer.resize(buffer.size() + PATH_MAX);
        }
        return convertFromANSI(afl::base::Memory<char>::unsafeCreate(&buffer[0], size));
    }
}

#else
int g_variableToMakeWin32FileSystemObjectFileNotEmpty;
#endif
