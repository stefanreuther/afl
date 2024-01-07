/**
  *  \file test/afl/sys/mutextest.cpp
  *  \brief Test for afl::sys::Mutex
  */

#include "afl/sys/mutex.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST_NOARG("afl.sys.Mutex")
{
    /* Test that we can create a mutex the way it's meant to. */
    afl::sys::Mutex mtx;
    mtx.wait();
    mtx.post();
}
