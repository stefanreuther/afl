/**
  *  \file arch/error.hpp
  *  \brief System-dependant part of afl/sys/error.cpp
  */
#ifndef AFL_ARCH_ERROR_HPP
#define AFL_ARCH_ERROR_HPP

#include "afl/base/types.hpp"
#include "afl/string/string.hpp"

#if TARGET_OS_POSIX
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include "arch/posix/posix.hpp"
namespace {
    pthread_mutex_t g_errorMutex = PTHREAD_MUTEX_INITIALIZER;

    inline int32_t
    errorNumber()
    {
        return errno;
    }

    inline String_t
    errorText(int32_t errorCode)
    {
        // strerror is not re-entrant, but unlike strerror_r,
        // it's portable and does not require us to provide a fixed-size buffer.
        // So let's just do the locking manually.
        struct Locker {
            Locker()
                { pthread_mutex_lock(&g_errorMutex); }
            ~Locker()
                { pthread_mutex_unlock(&g_errorMutex); }
        } locker;

        arch::posix::setupLocale();

        // Convert error code (but only if it's representable).
        char* resultNTBS = (errorCode == int(errorCode) ? strerror(errorCode) : 0);

        // Return result. Errors have the correct format, and just must be converted to UTF-8.
        return arch::posix::convertExternalToUtf8(afl::string::toMemory(resultNTBS));
    }
}
#elif TARGET_OS_WIN32
/*
 *  Win32 Version
 */
# define NOMINMAX
# include <windows.h>
# include <string.h>
# include <stdio.h>
# include "arch/win32/win32.hpp"
namespace {
    inline int32_t
    errorNumber()
    {
        return GetLastError();
    }

    inline String_t
    errorText(int32_t errorCode)
    {
        String_t result;
        if (arch::win32::hasUnicodeSupport()) {
            // Unicode version
            wchar_t* msg = 0;
            if (FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                               0, errorCode,
                               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                               (LPWSTR) &msg,
                               0, 0) != 0)
            {
                if (msg != 0) {
                    result = arch::win32::convertFromUnicode(afl::base::Memory<const wchar_t>::unsafeCreate(msg, wcslen(msg)));
                }
            }
            if (msg) {
                LocalFree(msg);
            }
        } else {
            // ANSI version
            char* msg = 0;
            if (FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                               0, errorCode,
                               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                               (LPSTR) &msg,
                               0, 0) != 0)
            {
                result = arch::win32::convertFromANSI(afl::string::toMemory(msg));
            }
            if (msg) {
                LocalFree(msg);
            }
        }

        // We expect the message to not end with a linefeed, we add that ourselves.
        // Windows messages have CR+LF at the end.
        String_t::size_type n = result.size();
        while (n > 0 && (result[n-1] == '\n' || result[n-1] == '\r')) {
            --n;
        }
        result.erase(n);

        // Fallback if Windows doesn't have a text
        if (result.size() == 0) {
            char tmp[100];
            sprintf(tmp, "<error 0x%08X>", unsigned(errorCode));
            result = tmp;
        }

        return result;
    }
}
#else
# error Teach me about your errors
#endif

#endif
