/**
  *  \file arch/interrupt.hpp
  *  \brief System-dependant part of afl/async/interrupt.cpp
  */
#ifndef AFL_ARCH_INTERRUPT_HPP
#define AFL_ARCH_INTERRUPT_HPP

#if TARGET_OS_POSIX
/*
 *  POSIX
 */
# include "arch/posix/posixinterrupt.hpp"
namespace {
    typedef arch::posix::PosixInterrupt Interrupt_t;
}
#elif TARGET_OS_WIN32
/*
 *  Win32
 */
# include "arch/win32/win32interrupt.hpp"
namespace {
    typedef arch::win32::Win32Interrupt Interrupt_t;
}
#else
# error Teach me about your interrupts
#endif

#endif
