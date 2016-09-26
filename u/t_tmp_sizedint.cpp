/**
  *  \file u/t_tmp_sizedint.cpp
  *  \brief Test for afl::tmp::SizedInt
  */

#include "afl/tmp/sizedint.hpp"

#include "u/t_tmp.hpp"

/** Simple test. */
void
TestTmpSizedInt::testIt()
{
    afl::tmp::SizedInt<4>::Type t4;
    TS_ASSERT_EQUALS(sizeof(t4), 4U);
    t4 = 0;
    TS_ASSERT_EQUALS(t4, 0);
    t4 = -10;
    TS_ASSERT_EQUALS(t4, -10);

    afl::tmp::SizedInt<8>::Type t8;
    TS_ASSERT_EQUALS(sizeof(t8), 8U);

    afl::tmp::SizedInt<2>::Type t2;
    TS_ASSERT_EQUALS(sizeof(t2), 2U);

    // This will invoke the default, and fail to compile if it doesn't:
    afl::tmp::SizedInt<9999, int[2]>::Type t9999;
    t9999[0] = 1;
    (void) t9999;
}
