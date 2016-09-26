/**
  *  \file u/t_base_staticassert.cpp
  *  \brief Test for static_assert
  */

#include "afl/base/staticassert.hpp"

#include "u/t_base.hpp"

/** Simple test. */
void
TestBaseStaticAssert::testIt()
{
    static_assert(1 + 1 == 2, "Math");
}
