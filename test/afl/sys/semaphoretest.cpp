/**
  *  \file test/afl/sys/semaphoretest.cpp
  *  \brief Test for afl::sys::Semaphore
  */

#include "afl/sys/semaphore.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.sys.Semaphore", a)
{
    afl::sys::Semaphore sem(1);
    sem.wait();

    a.check("01. wait", !sem.wait(100));
    a.check("02. wait", !sem.wait(0));
    sem.post();

    a.check("11. wait", sem.wait(100));
    a.check("11. wait", !sem.wait(0));
}
