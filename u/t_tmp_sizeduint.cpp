/**
  *  \file u/t_tmp_sizeduint.cpp
  *  \brief Test for afl::tmp::SizedUInt
  */

#include "afl/tmp/sizeduint.hpp"

#include "u/t_tmp.hpp"

/** Simple test. */
void
TestTmpSizedUInt::testIt()
{
    afl::tmp::SizedUInt<4>::Type t4;
    TS_ASSERT_EQUALS(sizeof(t4), 4U);
    t4 = 0;
    TS_ASSERT_EQUALS(t4, 0U);
    --t4;
    TS_ASSERT_EQUALS(t4, 0xFFFFFFFFU);

    afl::tmp::SizedUInt<8>::Type t8;
    TS_ASSERT_EQUALS(sizeof(t8), 8U);

    afl::tmp::SizedUInt<2>::Type t2;
    TS_ASSERT_EQUALS(sizeof(t2), 2U);

    // This will invoke the default, and fail to compile if it doesn't:
    afl::tmp::SizedUInt<9999, int[2]>::Type t9999;
    t9999[0] = 1;
    (void) t9999;
}
