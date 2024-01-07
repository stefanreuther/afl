/**
  *  \file test/afl/base/refcountedtest.cpp
  *  \brief Test for afl::base::RefCounted
  */

#include "afl/base/refcounted.hpp"
#include "afl/test/testrunner.hpp"

namespace {
    class Tester : public afl::base::RefCounted {
    };
}

/** Simple tests. */
AFL_TEST("afl.base.RefCounted", a) {
    // Create one object and make sure it works.
    Tester t1;
    a.checkEqual("1", static_cast<uint32_t>(t1.refCounter()), 0U);
    ++t1.refCounter();
    a.checkEqual("2", static_cast<uint32_t>(t1.refCounter()), 1U);

    // Create another object. Reference count must not be copied.
    Tester t2(t1);
    a.checkEqual("3", static_cast<uint32_t>(t2.refCounter()), 0U);
    a.checkEqual("4", static_cast<uint32_t>(t1.refCounter()), 1U);

    // Assign.
    t2 = t1;
    a.checkEqual("5", static_cast<uint32_t>(t2.refCounter()), 0U);
    a.checkEqual("6", static_cast<uint32_t>(t1.refCounter()), 1U);

    t1 = t2;
    a.checkEqual("7", static_cast<uint32_t>(t2.refCounter()), 0U);
    a.checkEqual("8", static_cast<uint32_t>(t1.refCounter()), 1U);
}
