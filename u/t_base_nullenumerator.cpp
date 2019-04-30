/**
  *  \file u/t_base_nullenumerator.cpp
  *  \brief Test for afl::base::NullEnumerator
  */

#include "afl/base/nullenumerator.hpp"

#include "t_base.hpp"

/** Simple test. */
void
TestBaseNullEnumerator::testIt()
{
    afl::base::NullEnumerator<int> testee;
    int n = 99;
    TS_ASSERT_EQUALS(testee.getNextElement(n), false);
    TS_ASSERT_EQUALS(n, 99);
}

