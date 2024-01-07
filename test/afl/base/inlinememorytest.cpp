/**
  *  \file test/afl/base/inlinememorytest.cpp
  *  \brief Test for afl::base::InlineMemory
  */

#include "afl/base/inlinememory.hpp"
#include "afl/test/testrunner.hpp"

/** Simple tests. */
AFL_TEST("afl.base.InlineMemory", as) {
    afl::base::InlineMemory<uint8_t,7> a;
    afl::base::InlineMemory<uint8_t,3> b;

    // Initial state
    as.checkEqual("initial size a", a.size(), 7U);
    as.checkEqual("initial size b", b.size(), 3U);
    a.fill(1);
    b.fill(2);
    as.checkEqual("value a", *a.at(0), 1);
    as.checkEqual("value b", *b.at(0), 2);

    // Copying copies the pointers
    afl::base::Memory<uint8_t> aa = a;
    a = b;
    as.checkEqual("address after copy a/b", a.at(0), b.at(0));
    as.checkDifferent("address after copy a/aa", a.at(0), aa.at(0));
    as.checkEqual("content after copy a", *a.at(0), 2);
    as.checkEqual("content after copy aa", *aa.at(0), 1);

    // reset clears
    a.reset();
    as.checkEqual("size after reset", a.size(), 0U);
}

