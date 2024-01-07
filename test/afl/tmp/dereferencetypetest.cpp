/**
  *  \file test/afl/tmp/dereferencetypetest.cpp
  *  \brief Test for afl::tmp::DereferenceType
  */

#include "afl/tmp/dereferencetype.hpp"

#include "afl/tmp/issametype.hpp"
#include "afl/test/testrunner.hpp"

// Test using the compiler
AFL_TEST_NOARG("afl.tmp.DereferenceType:compiled")
{
    int* a;
    afl::tmp::DereferenceType<int*>::Type b;
    a = &b;
    (void) a;
}

// Test using meta-programming (with IsSameType)
AFL_TEST("afl.tmp.DereferenceType:meta", a)
{
    a.check("01", (afl::tmp::IsSameType<afl::tmp::DereferenceType<int*>::Type, int>::result));
    a.check("02", (afl::tmp::IsSameType<afl::tmp::DereferenceType<int* const>::Type, int>::result));
}
