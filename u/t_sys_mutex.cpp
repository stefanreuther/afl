/**
  *  \file u/t_sys_mutex.cpp
  *  \brief Test for afl::sys::Mutex
  */

#include "afl/sys/mutex.hpp"
#include "u/t_sys.hpp"

/** Simple test. */
void
TestSysMutex::testIt()
{
    /* Test that we can create a mutex the way it's meant to. */
    afl::sys::Mutex mtx;
    mtx.wait();
    mtx.post();
}
