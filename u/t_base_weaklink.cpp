/**
  *  \file u/t_base_weaklink.cpp
  *  \brief Test for afl::base::WeakLink
  */

#include <memory>
#include "afl/base/weaklink.hpp"

#include "t_base.hpp"
#include "afl/base/weaktarget.hpp"

namespace {
    class Tester : public afl::base::WeakTarget { };
}

/** Simple test.
    Tests that removal in different sequences works.
    This is essentially the same test as TestWeakLink. */
void
TestBaseWeakLink::testIt()
{
    for (int i = 0; i < 32; ++i) {
        // Create WeakTarget
        std::auto_ptr<Tester> t0(new Tester());

        // Create a few pointers; some by copying other WeakLinks, some by pointing at the WeakTarget
        std::auto_ptr<afl::base::WeakLink<Tester> > t1(new afl::base::WeakLink<Tester>(t0.get()));
        std::auto_ptr<afl::base::WeakLink<Tester> > t2(new afl::base::WeakLink<Tester>(t0.get()));
        std::auto_ptr<afl::base::WeakLink<Tester> > t3(new afl::base::WeakLink<Tester>(*t2));
        std::auto_ptr<afl::base::WeakLink<Tester> > t4(new afl::base::WeakLink<Tester>(t0.get()));

        // Remove the objects in different sequence.
        if (i & 1) {
            t0.reset();
            TS_ASSERT(t1->get() == 0);
            TS_ASSERT(t2->get() == 0);
            TS_ASSERT(t3->get() == 0);
            TS_ASSERT(t4->get() == 0);
        }
        if (i & 2) {
            t1.reset();
            TS_ASSERT(t2->get() == t0.get());
            TS_ASSERT(t3->get() == t0.get());
            TS_ASSERT(t4->get() == t0.get());
        }
        if (i & 4) {
            t2.reset();
            TS_ASSERT(t1.get() == 0 || t1->get() == t0.get());
            TS_ASSERT(t3.get() == 0 || t3->get() == t0.get());
            TS_ASSERT(t4.get() == 0 || t4->get() == t0.get());
        }
        if (i & 8) {
            t3.reset();
            TS_ASSERT(t1.get() == 0 || t1->get() == t0.get());
            TS_ASSERT(t2.get() == 0 || t2->get() == t0.get());
            TS_ASSERT(t4.get() == 0 || t4->get() == t0.get());
        }
        if (i & 16) {
            t4.reset();
            TS_ASSERT(t1.get() == 0 || t1->get() == t0.get());
            TS_ASSERT(t2.get() == 0 || t2->get() == t0.get());
            TS_ASSERT(t3.get() == 0 || t3->get() == t0.get());
        }
    }
}

/** Test assignments. */
void
TestBaseWeakLink::testAssignment()
{
    Tester target;
    afl::base::WeakLink<Tester> t(&target);
    afl::base::WeakLink<Tester> t2;

    TS_ASSERT(t.get() == &target);

    TS_ASSERT(t2.get() == 0);

    t2 = t;
    TS_ASSERT(t2.get() == &target);

    t = 0;
    TS_ASSERT(t.get() == 0);
    TS_ASSERT(t2.get() == &target);

    t = &target;
    TS_ASSERT(t.get() == &target);
}
