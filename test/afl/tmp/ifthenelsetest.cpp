/**
  *  \file test/afl/tmp/ifthenelsetest.cpp
  *  \brief Test for afl::tmp::IfThenElse
  */

#include "afl/tmp/ifthenelse.hpp"

#include "afl/base/staticassert.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST_NOARG("afl.tmp.IfThenElse")
{
    typedef int A;
    typedef int B[2];

    // Check basic assumptions
    static_assert(sizeof(A) != sizeof(B), "Must be distinguishable types");

    // Check IfThenElse
    static_assert(sizeof(afl::tmp::IfThenElse<1 + 1 == 2, A, B>::Type) == sizeof(A), "true");
    static_assert(sizeof(afl::tmp::IfThenElse<1 + 1 == 3, A, B>::Type) == sizeof(B), "false");
}
