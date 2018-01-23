/**
  *  \file arch/win32/win32.hpp
  *  \brief Win32 specifics
  */
#ifndef AFL_ARCH_WIN32_WIN32_HPP
#define AFL_ARCH_WIN32_WIN32_HPP

#if TARGET_OS_WIN32
#include <vector>
#include "afl/string/string.hpp"
#include "afl/base/memory.hpp"

namespace arch { namespace win32 {

    /** Check for Unicode support.
        Note that even if you decide not to call a "W" syscall,
        the mere presence in the source code requires a linkable symbol for the program to start.
        Unfortunately, not all "W" syscalls have one in Win95 DLLs.

        \retval true System has Unicode support, use "W" syscalls
        \retval false System does not have Unicode support, use "A" syscalls */
    bool hasUnicodeSupport();

    /** Convert Unicode string to UTF-8.
        \param msg Descriptor for string
        \return converted string */
    String_t convertFromUnicode(afl::base::Memory<const wchar_t> msg);

    /** Convert ANSI (CP_ACP) string to UTF-8.
        \param msg Descriptor for string
        \return converted string */
    String_t convertFromANSI(afl::base::Memory<const char> msg);

    /** Convert codepage string to UTF-8.
        \param msg Descriptor for string
        \param codepage Codepage
        \return converted string */
    String_t convertFromCodepage(afl::base::Memory<const char> msg, unsigned int codepage);

    /** Unicode string. */
    typedef std::vector<wchar_t> WStr;

    /** Convert UTF-8 string to Unicode.
        \param out [out] Output is produced here
        \param msg [in] Input string, UTF-8 */
    void convertToUnicode(WStr& out, afl::base::Memory<const char> msg);

    /** Convert UTF-8 string to ANSI (CP_ACP).
        \param msg [in] Input string, UTF-8
        \return converted string */
    String_t convertToANSI(afl::base::Memory<const char> msg);

    /** Convert UTF-8 string to specific codepage.
        \param msg [in] Input string, UTF-8
        \param codepage [in] Desired codepage
        \return converted string */
    String_t convertToCodepage(afl::base::Memory<const char> msg, unsigned int codepage);

    /** Get shell folder.
        Shortcut for SHELLAPI call sequence.
        \param csidl Folder ID (CSIDL_xxx). */
    String_t getShellFolder(int csidl);

    /** Zero-terminate an unicode string.
        \out [in/out] String to test/update
        \retval false String contains embedded nulls and cannot be zero-terminated
        \retval true Success */
    bool terminateUnicode(WStr& str);

} }
#else
# error Invalid operating system
#endif

#endif
