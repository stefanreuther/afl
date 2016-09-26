/**
  *  \file u/t_base_refcounted.cpp
  *  \brief Test for afl::base::RefCounted
  */

#include "afl/base/refcounted.hpp"

#include "u/t_base.hpp"

namespace {
    class Tester : public afl::base::RefCounted {
    };
}

/** Simple tests. */
void
TestBaseRefCounted::testIt()
{
    // Create one object and make sure it works.
    Tester t1;
    TS_ASSERT_EQUALS(t1.refCounter(), 0U);
    ++t1.refCounter();
    TS_ASSERT_EQUALS(t1.refCounter(), 1U);

    // Create another object. Reference count must not be copied.
    Tester t2(t1);
    TS_ASSERT_EQUALS(t2.refCounter(), 0U);
    TS_ASSERT_EQUALS(t1.refCounter(), 1U);

    // Assign.
    t2 = t1;
    TS_ASSERT_EQUALS(t2.refCounter(), 0U);
    TS_ASSERT_EQUALS(t1.refCounter(), 1U);

    t1 = t2;
    TS_ASSERT_EQUALS(t2.refCounter(), 0U);
    TS_ASSERT_EQUALS(t1.refCounter(), 1U);
}
