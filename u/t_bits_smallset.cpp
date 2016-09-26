/**
  *  \file u/t_bits_smallset.cpp
  *  \brief Test for afl::bits::SmallSet
  */

#include "afl/bits/smallset.hpp"

#include "u/t_bits.hpp"

namespace {
    enum Numbers { One, Two, Three };
}

/** Simple tests. */
void
TestBitsSmallSet::testIt()
{
    typedef afl::bits::SmallSet<Numbers> Num_set;

    Num_set e;
    Num_set s1(One), s2(Two), s3(Three);

    TS_ASSERT(e.empty());
    TS_ASSERT(!s1.empty());
    TS_ASSERT(!s2.empty());
    TS_ASSERT(!s3.empty());

    TS_ASSERT(s1.contains(One));
    TS_ASSERT(!s1.contains(Two));
    TS_ASSERT(!s1.contains(Three));

    TS_ASSERT(s1 == One);
    TS_ASSERT(s2 == Two);
    TS_ASSERT(s3 == Three);

    TS_ASSERT(s3 != One);
    TS_ASSERT(s1 != Two);
    TS_ASSERT(s2 != Three);

    TS_ASSERT((s1 + s2).contains(One));
    TS_ASSERT(!(s1 + s2).contains(Three));
    TS_ASSERT((s1 + s2).contains(s1));
    TS_ASSERT((s1 + s2).contains(s2));
    TS_ASSERT(!(s1 + s2).contains(s3));

    TS_ASSERT_DIFFERS(s1, s2);
    TS_ASSERT_EQUALS(s1 + Two - One, s2);

    Num_set n(s1);
    n += Two;
    TS_ASSERT(n.containsAnyOf(s1));
    TS_ASSERT(n.contains(s1));
    TS_ASSERT(!s1.contains(n));

    TS_ASSERT_EQUALS((n ^ s1), s2);
    TS_ASSERT_EQUALS((s1 ^ s2), n);
    TS_ASSERT_EQUALS((n ^ s2), s1);

    TS_ASSERT((s1 & s2).empty());
    TS_ASSERT(!(s1 - s2).empty());

    TS_ASSERT_EQUALS((n - s2), s1);
}
