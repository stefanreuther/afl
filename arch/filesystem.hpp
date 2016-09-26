/**
  *  \file arch/filesystem.hpp
  *  \brief System-dependant part of afl/io/filesystem.cpp
  */
#ifndef AFL_ARCH_FILESYSTEM_HPP
#define AFL_ARCH_FILESYSTEM_HPP

#if TARGET_OS_POSIX
/*
 *  POSIX
 */
# include "arch/posix/posixfilesystem.hpp"
namespace {
    typedef arch::posix::PosixFileSystem FileSystem_t;
}
#elif TARGET_OS_WIN32
/*
 *  Win32
 */
# include "arch/win32/win32filesystem.hpp"
namespace {
    typedef arch::win32::Win32FileSystem FileSystem_t;
}
#else
# error Teach me about your file system
#endif

#endif
