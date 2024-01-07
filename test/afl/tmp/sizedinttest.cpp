/**
  *  \file test/afl/tmp/sizedinttest.cpp
  *  \brief Test for afl::tmp::SizedInt
  */

#include "afl/tmp/sizedint.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.tmp.SizedInt", a)
{
    afl::tmp::SizedInt<4>::Type t4;
    a.checkEqual("type 4", sizeof(t4), 4U);
    t4 = 0;
    a.checkEqual("zero 4", t4, 0);
    t4 = -10;
    a.checkEqual("neg 4", t4, -10);

    afl::tmp::SizedInt<8>::Type t8;
    a.checkEqual("type 8", sizeof(t8), 8U);

    afl::tmp::SizedInt<2>::Type t2;
    a.checkEqual("type 2", sizeof(t2), 2U);

    // This will invoke the default, and fail to compile if it doesn't:
    afl::tmp::SizedInt<9999, int[2]>::Type t9999;
    t9999[0] = 1;
    (void) t9999;
}
