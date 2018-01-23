/**
  *  \file arch/win32/win32dialog.cpp
  */

#if TARGET_OS_WIN32
#include <windows.h>
#include "arch/win32/win32dialog.hpp"
#include "arch/win32/win32.hpp"
#include "afl/string/string.hpp"

namespace arch { namespace win32 { namespace {
    void convertLF(String_t& str)
    {
        String_t::size_type n = 0, p;
        while ((p = str.find('\n', n)) != String_t::npos) {
            str.insert(p, "\r");
            n = p+2;
        }
    }

    int doDialog(UINT flags, String_t info, String_t title)
    {
        // Replace \n -> \r\n
        convertLF(info);
        convertLF(title);

        // Show it
        if (hasUnicodeSupport()) {
            WStr wInfo, wTitle;
            convertToUnicode(wInfo, afl::string::toMemory(info));
            convertToUnicode(wTitle, afl::string::toMemory(title));
            terminateUnicode(wInfo);
            terminateUnicode(wTitle);
            return MessageBoxW(NULL, &wInfo[0], &wTitle[0], flags);
        } else {
            String_t aInfo = convertToANSI(afl::string::toMemory(info));
            String_t aTitle = convertToANSI(afl::string::toMemory(title));
            return MessageBoxA(NULL, aInfo.c_str(), aTitle.c_str(), flags);
        }
    }
} } }

void
arch::win32::Win32Dialog::showInfo(String_t info, String_t title)
{
    doDialog(MB_OK | MB_ICONINFORMATION, info, title);
}

void
arch::win32::Win32Dialog::showError(String_t info, String_t title)
{
    doDialog(MB_OK | MB_ICONSTOP, info, title);
}

bool
arch::win32::Win32Dialog::askYesNo(String_t info, String_t title)
{
    return doDialog(MB_YESNO | MB_ICONQUESTION, info, title) == IDYES;
}
#else
int g_variableToMakeWin32DialogObjectFileNotEmpty;
#endif
