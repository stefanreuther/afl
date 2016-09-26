/**
  *  \file u/t_base_countof.cpp
  *  \brief Test for #countof
  */

#include <cstdlib>
#include "afl/base/countof.hpp"

#include "u/t_base.hpp"
#include "afl/base/staticassert.hpp"
#include "afl/base/types.hpp"

/** Simple tests. */
void
TestBaseCountof::testIt()
{
    int a[33];
    int b[17][4];

    // Regular assert
    TS_ASSERT_EQUALS(countof(a), 33U);
    TS_ASSERT_EQUALS(countof(b), 17U);
    TS_ASSERT_EQUALS(countof(b[0]), 4U);

    // Static assert
    static_assert(countof(a) == 33, "first");
    static_assert(countof(b) == 17, "second");
    static_assert(countof(b[0]) == 4, "third");

    // Switch/case
    size_t z = size_t(rand());
    switch (z) {
     case countof(a):    z = 1; break;
     case countof(b):    z = 2; break;
     case countof(b[0]): z = 3; break;
     default:            z = 4; break;
    }

    // Array dimensioning
    int aa[countof(a)];
    int bb[countof(b)][countof(b[0])];
    static_assert(sizeof(a) == sizeof(aa), "a");
    static_assert(sizeof(b) == sizeof(bb), "b");
}
