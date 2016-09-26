/**
  *  \file u/t_sys_semaphore.cpp
  *  \brief Test for afl::sys::Semaphore
  */

#include "afl/sys/semaphore.hpp"

#include "u/t_sys.hpp"

/** Simple test. */
void
TestSysSemaphore::testSimple()
{
    afl::sys::Semaphore sem(1);
    sem.wait();

    TS_ASSERT(!sem.wait(100));
    TS_ASSERT(!sem.wait(0));
    sem.post();

    TS_ASSERT(sem.wait(100));
    TS_ASSERT(!sem.wait(0));
}
