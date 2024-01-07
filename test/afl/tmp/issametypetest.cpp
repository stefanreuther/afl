/**
  *  \file test/afl/tmp/issametypetest.cpp
  *  \brief Test for afl::tmp::IsSameType
  */

#include "afl/tmp/issametype.hpp"

#include "afl/base/staticassert.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.tmp.IsSameType", a)
{
    a.checkEqual("01", (afl::tmp::IsSameType<int,int>::result),            true);
    a.checkEqual("02", (afl::tmp::IsSameType<int,signed int>::result),     true);
    a.checkEqual("03", (afl::tmp::IsSameType<int,unsigned int>::result),   false);

    a.checkEqual("11", (afl::tmp::IsSameType<char,signed char>::result),   false);
    a.checkEqual("12", (afl::tmp::IsSameType<char,unsigned char>::result), false);

    a.checkEqual("21", (afl::tmp::IsSameType<void,void>::result),          true);
    a.checkEqual("22", (afl::tmp::IsSameType<long,long>::result),          true);

    // It's actually a compile-time constant:
    static_assert((afl::tmp::IsSameType<int,signed int>::result), "int/signed");
    static_assert(!(afl::tmp::IsSameType<int,unsigned int>::result), "int/unsigned");
}
