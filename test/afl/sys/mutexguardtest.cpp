/**
  *  \file test/afl/sys/mutexguardtest.cpp
  *  \brief Test for afl::sys::MutexGuard
  */

#include "afl/sys/mutexguard.hpp"
#include "afl/test/testrunner.hpp"

/** Simple test; also tests that Mutex is recursive. */
AFL_TEST_NOARG("afl.sys.MutexGuard")
{
    afl::sys::Mutex mtx;
    afl::sys::MutexGuard g(mtx);
    afl::sys::MutexGuard g2(mtx);
    afl::sys::MutexGuard g3(mtx);
}
