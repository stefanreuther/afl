/**
  *  \file test/afl/base/weaklinktest.cpp
  *  \brief Test for afl::base::WeakLink
  */

#include "afl/base/weaklink.hpp"
#include "afl/test/testrunner.hpp"

namespace {
    class Tester : public afl::base::WeakTarget { };
}

/** Simple test.
    Tests that removal in different sequences works.
    This is essentially the same test as BaseWeakLink. */
AFL_TEST("afl.base.WeakLink", a)
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
            a.check("1/t1", t1->get() == 0);
            a.check("1/t2", t2->get() == 0);
            a.check("1/t3", t3->get() == 0);
            a.check("1/t4", t4->get() == 0);
        }
        if (i & 2) {
            t1.reset();
            a.check("2/t2", t2->get() == t0.get());
            a.check("2/t3", t3->get() == t0.get());
            a.check("2/t4", t4->get() == t0.get());
        }
        if (i & 4) {
            t2.reset();
            a.check("4/t1", t1.get() == 0 || t1->get() == t0.get());
            a.check("4/t3", t3.get() == 0 || t3->get() == t0.get());
            a.check("4/t4", t4.get() == 0 || t4->get() == t0.get());
        }
        if (i & 8) {
            t3.reset();
            a.check("8/t1", t1.get() == 0 || t1->get() == t0.get());
            a.check("8/t2", t2.get() == 0 || t2->get() == t0.get());
            a.check("8t/4", t4.get() == 0 || t4->get() == t0.get());
        }
        if (i & 16) {
            t4.reset();
            a.check("16/t1", t1.get() == 0 || t1->get() == t0.get());
            a.check("16/t2", t2.get() == 0 || t2->get() == t0.get());
            a.check("16/t3", t3.get() == 0 || t3->get() == t0.get());
        }
    }
}

/** Test assignments. */
AFL_TEST("afl.base.WeakLink:assign", a)
{
    Tester target;
    afl::base::WeakLink<Tester> t(&target);
    afl::base::WeakLink<Tester> t2;

    a.check("01", t.get() == &target);

    a.check("02", t2.get() == 0);

    t2 = t;
    a.check("03", t2.get() == &target);

    t = 0;
    a.check("04", t.get() == 0);
    a.check("05", t2.get() == &target);

    t = &target;
    a.check("06", t.get() == &target);
}
