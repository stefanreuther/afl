/**
  *  \file test/afl/tmp/sizeduinttest.cpp
  *  \brief Test for afl::tmp::SizedUInt
  */

#include "afl/tmp/sizeduint.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.tmp.SizedUInt", a)
{
    afl::tmp::SizedUInt<4>::Type t4;
    a.checkEqual("type 4", sizeof(t4), 4U);
    t4 = 0;
    a.checkEqual("zero 4", t4, 0U);
    --t4;
    a.checkEqual("decr 4", t4, 0xFFFFFFFFU);

    afl::tmp::SizedUInt<8>::Type t8;
    a.checkEqual("type 8", sizeof(t8), 8U);

    afl::tmp::SizedUInt<2>::Type t2;
    a.checkEqual("type 2", sizeof(t2), 2U);

    // This will invoke the default, and fail to compile if it doesn't:
    afl::tmp::SizedUInt<9999, int[2]>::Type t9999;
    t9999[0] = 1;
    (void) t9999;
}
