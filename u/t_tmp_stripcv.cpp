/**
  *  \file u/t_tmp_stripcv.cpp
  *  \brief Test for afl::tmp::StripCV
  */

#include "afl/tmp/stripcv.hpp"

#include "u/t_tmp.hpp"

/** Simple test. */
void
TestTmpStripCV::testIt()
{
    afl::tmp::StripCV<int>::Type* p1 = 0;
    afl::tmp::StripCV<const int>::Type* p2 = 0;
    afl::tmp::StripCV<volatile int>::Type* p3 = 0;
    afl::tmp::StripCV<const volatile int>::Type* p4 = 0;

    // Assign each pointer to each other. Must all work, since they have been converted to 'int*'.
    // The asserts are so that the compiler doesn't complain about useless stores.
    p1 = p1; TS_ASSERT(p1 == 0);
    p1 = p2; TS_ASSERT(p1 == 0);
    p1 = p3; TS_ASSERT(p1 == 0);
    p1 = p4; TS_ASSERT(p1 == 0);

    p2 = p1; TS_ASSERT(p2 == 0);
    p2 = p2; TS_ASSERT(p2 == 0);
    p2 = p3; TS_ASSERT(p2 == 0);
    p2 = p4; TS_ASSERT(p2 == 0);

    p3 = p1; TS_ASSERT(p3 == 0);
    p3 = p2; TS_ASSERT(p3 == 0);
    p3 = p3; TS_ASSERT(p3 == 0);
    p3 = p4; TS_ASSERT(p3 == 0);

    p4 = p1; TS_ASSERT(p4 == 0);
    p4 = p2; TS_ASSERT(p4 == 0);
    p4 = p3; TS_ASSERT(p4 == 0);
    p4 = p4; TS_ASSERT(p4 == 0);

}
