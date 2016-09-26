/**
  *  \file arch/filemapping.hpp
  *  \brief System-dependant part of afl/io/filemapping.cpp
  *
  *  Porting interface: port must implement a class with a static createSystemFileMapping() method
  *  that fulfils the signature and contract of FileMapping::createDirect(),
  *  and a typedef SystemFileMapping_t pointing to that class.
  *
  *  In particular, the createSystemFileMapping() method must
  *  - accept all parameter combinations (misalignment)
  *  - return 0 if anything fails
  *  - advance the file pointer if the map succeeds
  *
  *  The abovementioned class can be the FileMapping descendant, but doesn't have to.
  *  The method is called createSystemFileMapping() to avoid accidental loops through FileMapping::create().
  *
  *  If the operating system has no mmap() facility, the createSystemFileMapping() just always returns 0.
  */
#ifndef AFL_ARCH_FILEMAPPING_HPP
#define AFL_ARCH_FILEMAPPING_HPP

#if TARGET_OS_POSIX
/*
 *  POSIX
 */
# include "arch/posix/posixfilemapping.hpp"
namespace {
    typedef arch::posix::PosixFileMapping SystemFileMapping_t;
}
#elif TARGET_OS_WIN32
/*
 *  Win32
 */
# include "arch/win32/win32filemapping.hpp"
namespace {
    typedef arch::win32::Win32FileMapping SystemFileMapping_t;
}
#else
# error Teach me about your file mappings
#endif

#endif
