/**
  *  \file test/afl/base/nullenumeratortest.cpp
  *  \brief Test for afl::base::NullEnumerator
  */

#include "afl/base/nullenumerator.hpp"
#include "afl/test/testrunner.hpp"

/** Simple test. */
AFL_TEST("afl.base.NullEnumerator", a) {
    afl::base::NullEnumerator<int> testee;
    int n = 99;
    a.checkEqual("no getNextElement", testee.getNextElement(n), false);
    a.checkEqual("unchanged value", n, 99);
}
