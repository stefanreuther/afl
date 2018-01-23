/**
  *  \file arch/win32/win32.cpp
  *  \brief Win32 specifics
  */

#if TARGET_OS_WIN32
#define NOMINMAX
#define CINTERFACE
#include <vector>
#include <windows.h>
#include <shlobj.h>
#include "arch/win32/win32.hpp"
#include "afl/charset/utf8.hpp"
#include "afl/charset/utf8reader.hpp"

namespace {
    LPMALLOC getShellAllocator()
    {
        static LPMALLOC m = 0;
        if (!m) {
            SHGetMalloc(&m);
        }
        return m;
    }
}

// Check for Unicode support.
bool
arch::win32::hasUnicodeSupport()
{
    // The highest bit is 0 on Windows NT, 1 on Win95/Win32s.
    static DWORD version = GetVersion();
    return version <= 0x7FFFFFFFU;
}

// Convert Unicode string to UTF-8.
String_t
arch::win32::convertFromUnicode(afl::base::Memory<const wchar_t> msg)
{
    String_t result;
    afl::charset::Utf8 u8;
    while (const wchar_t* ch = msg.eat()) {
        u8.append(result, *ch);
    }
    return result;
}

// Convert ANSI (CP_ACP) string to UTF-8.
String_t
arch::win32::convertFromANSI(afl::base::Memory<const char> msg)
{
    return convertFromCodepage(msg, CP_ACP);
}

// Convert codepage string to UTF-8.
String_t
arch::win32::convertFromCodepage(afl::base::Memory<const char> msg, unsigned int codepage)
{
    int sizeNeeded = MultiByteToWideChar(codepage, 0, msg.unsafeData(), msg.size(), 0, 0);
    if (sizeNeeded != 0) {
        std::vector<wchar_t> wstr(sizeNeeded);
        MultiByteToWideChar(codepage, 0, msg.unsafeData(), msg.size(), &wstr[0], sizeNeeded);
        return convertFromUnicode(afl::base::Memory<const wchar_t>::unsafeCreate(&wstr[0], sizeNeeded));
    } else {
        return String_t();
    }
}

// Convert UTF-8 string to Unicode.
void
arch::win32::convertToUnicode(WStr& out, afl::base::Memory<const char> msg)
{
    afl::charset::Utf8Reader rdr(msg.toBytes(), 0);
    while (rdr.hasMore()) {
        // FIXME: handle UTF-16!
        out.push_back(wchar_t(rdr.eat()));
    }
}

// Convert UTF-8 string to ANSI (CP_ACP).
String_t
arch::win32::convertToANSI(afl::base::Memory<const char> msg)
{
    return convertToCodepage(msg, CP_ACP);
}

// Convert UTF-8 string to specific codepage.
String_t
arch::win32::convertToCodepage(afl::base::Memory<const char> msg, unsigned int codepage)
{
    // Convert to Unicode
    WStr wstr;
    convertToUnicode(wstr, msg);

    // Convert Unicode to codepage
    int sizeNeeded = WideCharToMultiByte(codepage, 0, &wstr[0], wstr.size(), 0, 0, 0, 0);
    std::vector<char> astr(sizeNeeded);
    WideCharToMultiByte(codepage, 0, &wstr[0], wstr.size(), &astr[0], sizeNeeded, 0, 0);
    return String_t(&astr[0], sizeNeeded);
}

// Get shell folder.
String_t
arch::win32::getShellFolder(int csidl)
{
    LPITEMIDLIST il;
    char buffer[MAX_PATH];

    if (!SUCCEEDED(SHGetSpecialFolderLocation(NULL, csidl, &il))) {
        return String_t();
    }

    /* Windows 95 doesn't have SHGetPathFromIDListW. It doesn't even
       have a stub in its shell32.dll, so that dynamic linking fails.
       We therefore always use the ANSI version, under the assumption
       that the well-known folders are always stored using names
       representable in the ANSI codepage. */
    BOOL rv = SHGetPathFromIDList(il, buffer);
    if (LPMALLOC m = getShellAllocator()) {
        m->lpVtbl->Free(m, il);
    }
    if (rv) {
        return convertFromANSI(afl::string::toMemory(buffer));
    } else {
        return String_t();
    }
}

// Zero-terminate an unicode string.
bool
arch::win32::terminateUnicode(WStr& str)
{
    for (size_t i = 0, n = str.size(); i < n; ++i) {
        if (str[i] == L'\0') {
            return false;
        }
    }
    str.push_back(L'\0');
    return true;
}

#else
int g_variableToMakeWin32ObjectFileNotEmpty;
#endif
