/**
  *  \file arch/dialog.hpp
  */
#ifndef AFL_ARCH_DIALOG_HPP
#define AFL_ARCH_DIALOG_HPP

#if TARGET_OS_POSIX
/*
 *  POSIX
 */
# include "arch/posix/posixdialog.hpp"
namespace {
    typedef arch::posix::PosixDialog Dialog_t;
}
#elif TARGET_OS_WIN32
/*
 *  Win32
 */
# include "arch/win32/win32dialog.hpp"
namespace {
    typedef arch::win32::Win32Dialog Dialog_t;
}
#else
# error Teach me about your file dialogs
#endif

#endif
