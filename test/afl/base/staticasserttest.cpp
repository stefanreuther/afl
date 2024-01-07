/**
  *  \file test/afl/base/staticasserttest.cpp
  *  \brief Test for afl::base::StaticAssert
  */

#include "afl/base/staticassert.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST_NOARG("afl.base.static_assert")
{
    static_assert(1 + 1 == 2, "Math");
}
