/**
  *  \file u/t_tmp_ifthenelse.cpp
  *  \brief Test for afl::tmp::IfThenElse
  */

#include "afl/tmp/ifthenelse.hpp"

#include "u/t_tmp.hpp"
#include "afl/base/staticassert.hpp"

/** Simple test. */
void
TestTmpIfThenElse::testIt()
{
    typedef int A;
    typedef int B[2];

    // Check basic assumptions
    static_assert(sizeof(A) != sizeof(B), "Must be distinguishable types");

    // Check IfThenElse
    static_assert(sizeof(afl::tmp::IfThenElse<1 + 1 == 2, A, B>::Type) == sizeof(A), "true");
    static_assert(sizeof(afl::tmp::IfThenElse<1 + 1 == 3, A, B>::Type) == sizeof(B), "false");
}
