/**
  *  \file arch/networkstack.hpp
  *  \brief System-dependant Part of afl::net::NetworkStack
  */
#ifndef AFL_ARCH_NETWORKSTACK_HPP
#define AFL_ARCH_NETWORKSTACK_HPP

#if TARGET_OS_POSIX
/*
 *  POSIX
 */
# include "arch/posix/posixnetworkstack.hpp"
namespace {
    typedef arch::posix::PosixNetworkStack NetworkStack_t;
}
#elif TARGET_OS_WIN32
/*
 *  Win32
 */
# include "arch/win32/win32networkstack.hpp"
namespace {
    typedef arch::win32::Win32NetworkStack NetworkStack_t;
}
#else
# error Teach me about your network stack
#endif

#endif
