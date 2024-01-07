/**
  *  \file test/afl/base/baseweaklinktest.cpp
  *  \brief Test for afl::base::BaseWeakLink
  */

#include "afl/base/baseweaklink.hpp"
#include "afl/test/testrunner.hpp"
#include "afl/base/weaktarget.hpp"

using afl::base::BaseWeakLink;

namespace {
    class Tester : public afl::base::WeakTarget { };
}

/** Simple test.
    Tests that removal in different sequences works. */
AFL_TEST("afl.base.BaseWeakLink", a) {
    for (int i = 0; i < 32; ++i) {
        // Create WeakTarget
        std::auto_ptr<Tester> t0(new Tester());

        // Create a few pointers; some by copying other WeakLinks, some by pointing at the WeakTarget
        std::auto_ptr<BaseWeakLink> t1(new BaseWeakLink(t0.get()));
        std::auto_ptr<BaseWeakLink> t2(new BaseWeakLink(t0.get()));
        std::auto_ptr<BaseWeakLink> t3(new BaseWeakLink(*t2));
        std::auto_ptr<BaseWeakLink> t4(new BaseWeakLink(t0.get()));

        // Remove the objects in different sequence.
        if (i & 1) {
            t0.reset();
            a.checkNull("1/t1", t1->get());
            a.checkNull("1/t2", t2->get());
            a.checkNull("1/t3", t3->get());
            a.checkNull("1/t4", t4->get());
        }
        if (i & 2) {
            t1.reset();
            a.checkEqual("2/t2", t2->get(), t0.get());
            a.checkEqual("2/t3", t3->get(), t0.get());
            a.checkEqual("2/t4", t4->get(), t0.get());
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
            a.check("8/t4", t4.get() == 0 || t4->get() == t0.get());
        }
        if (i & 16) {
            t4.reset();
            a.check("16/t1", t1.get() == 0 || t1->get() == t0.get());
            a.check("16/t2", t2.get() == 0 || t2->get() == t0.get());
            a.check("16/t3", t3.get() == 0 || t3->get() == t0.get());
        }
    }
}
