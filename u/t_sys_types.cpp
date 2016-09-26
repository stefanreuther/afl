/**
  *  \file u/t_sys_types.cpp
  *  \brief Test for afl/sys/types.hpp
  */

#include "afl/sys/types.hpp"
#include "u/t_sys.hpp"

/** Test for Timeout_t. */
void
TestSysTypes::testTimeout()
{
    TS_ASSERT(afl::sys::INFINITE_TIMEOUT > 0);

    // 0 must be a representable time
    {
        afl::sys::Timeout_t t = 0;
        TS_ASSERT(t == 0);
    }

    // One day must be a representable time
    {
        afl::sys::Timeout_t t = 86400L * 1000L;
        TS_ASSERT(t == 86400 * 1000L);
    }

    // Ten days must be a representable time
    {
        afl::sys::Timeout_t t = 10L * 86400L * 1000L;
        TS_ASSERT(t == 10L * 86400L * 1000L);
    }

    // Maximum value
    {
        afl::sys::Timeout_t t = 0xFFFFFFFEL;
        TS_ASSERT(t == 0xFFFFFFFEL);
    }
}
