/**
  *  \file u/t_sys_mutexguard.cpp
  *  \brief Test for afl::sys::MutexGuard
  */

#include "afl/sys/mutexguard.hpp"

#include "u/t_sys.hpp"

/** Simple test. */
void
TestSysMutexGuard::testIt()
{
    afl::sys::Mutex mtx;
    afl::sys::MutexGuard g(mtx);
    afl::sys::MutexGuard g2(mtx);
    afl::sys::MutexGuard g3(mtx);
}
