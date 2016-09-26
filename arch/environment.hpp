/**
  *  \file arch/environment.hpp
  *  \brief System-dependant Part of afl/sys/environment.cpp
  */
#ifndef AFL_ARCH_ENVIRONMENT_HPP
#define AFL_ARCH_ENVIRONMENT_HPP

#include "afl/sys/environment.hpp"

#if TARGET_OS_POSIX
/*
 *  POSIX version
 */
#include "arch/posix/posixenvironment.hpp"
namespace {
    typedef arch::posix::PosixEnvironment Environment_t;
}
#elif TARGET_OS_WIN32
/*
 *  Win32 version
 */
#include "arch/win32/win32environment.hpp"
namespace {
    typedef arch::win32::Win32Environment Environment_t;
}
#else
# error Teach me about your environment
#endif

#endif
