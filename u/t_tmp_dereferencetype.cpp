/**
  *  \file u/t_tmp_dereferencetype.cpp
  *  \brief Test for afl::tmp::DereferenceType
  */

#include "afl/tmp/dereferencetype.hpp"

#include "u/t_tmp.hpp"
#include "afl/tmp/issametype.hpp"

/** Simple test. */
void
TestTmpDereferenceType::testIt()
{
    // Test code
    int* a;
    afl::tmp::DereferenceType<int*>::Type b;
    a = &b;
    (void) a;

    // Test with IsSameType
    TS_ASSERT((afl::tmp::IsSameType<afl::tmp::DereferenceType<int*>::Type, int>::result));
    TS_ASSERT((afl::tmp::IsSameType<afl::tmp::DereferenceType<int* const>::Type, int>::result));
}
