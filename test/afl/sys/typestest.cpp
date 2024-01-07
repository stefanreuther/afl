/**
  *  \file test/afl/sys/typestest.cpp
  *  \brief Test for afl::sys::Types
  */

#include "afl/sys/types.hpp"
#include "afl/test/testrunner.hpp"

/** Test for Timeout_t. */
AFL_TEST("afl.sys.Timeout_t", a)
{
    a.check("INFINITE_TIMEOUT", afl::sys::INFINITE_TIMEOUT > 0);

    // 0 must be a representable time
    {
        afl::sys::Timeout_t t = 0;
        a.check("0", t == 0);
    }

    // One day must be a representable time
    {
        afl::sys::Timeout_t t = 86400L * 1000L;
        a.check("day", t == 86400 * 1000L);
    }

    // Ten days must be a representable time
    {
        afl::sys::Timeout_t t = 10L * 86400L * 1000L;
        a.check("2 days", t == 10L * 86400L * 1000L);
    }

    // Maximum value
    {
        afl::sys::Timeout_t t = 0xFFFFFFFEL;
        a.check("max", t == 0xFFFFFFFEL);
    }
}
