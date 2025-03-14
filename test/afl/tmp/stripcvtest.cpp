/**
  *  \file test/afl/tmp/stripcvtest.cpp
  *  \brief Test for afl::tmp::StripCV
  */

#include "afl/tmp/stripcv.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.tmp.StripCV", a)
{
    afl::tmp::StripCV<int>::Type* p1 = 0;
    afl::tmp::StripCV<const int>::Type* p2 = 0;
    afl::tmp::StripCV<volatile int>::Type* p3 = 0;
    afl::tmp::StripCV<const volatile int>::Type* p4 = 0;

    // Assign each pointer to each other. Must all work, since they have been converted to 'int*'.
    // The asserts are so that the compiler doesn't complain about useless stores.
    p1 = *&p1; a.check("01", p1 == 0);
    p1 =   p2; a.check("02", p1 == 0);
    p1 =   p3; a.check("03", p1 == 0);
    p1 =   p4; a.check("04", p1 == 0);

    p2 =   p1; a.check("11", p2 == 0);
    p2 = *&p2; a.check("12", p2 == 0);
    p2 =   p3; a.check("13", p2 == 0);
    p2 =   p4; a.check("14", p2 == 0);

    p3 =   p1; a.check("21", p3 == 0);
    p3 =   p2; a.check("22", p3 == 0);
    p3 = *&p3; a.check("23", p3 == 0);
    p3 =   p4; a.check("24", p3 == 0);

    p4 =   p1; a.check("31", p4 == 0);
    p4 =   p2; a.check("32", p4 == 0);
    p4 =   p3; a.check("33", p4 == 0);
    p4 = *&p4; a.check("34", p4 == 0);
}
