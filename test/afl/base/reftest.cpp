/**
  *  \file test/afl/base/reftest.cpp
  *  \brief Test for afl::base::Ref
  */

#include "afl/base/ref.hpp"
#include "afl/test/testrunner.hpp"

/*
 *  Pointer to RefCounted object type
 */

namespace {
    class RefCountedObj : public afl::base::RefCounted {
        RefCountedObj(const RefCountedObj&);
        RefCountedObj& operator=(const RefCountedObj&);
     public:
        int n;
        static int live;
        RefCountedObj(int n = 0)
            : n(n) { ++live; }
        ~RefCountedObj()
            { --live; }
    };
    int RefCountedObj::live = 0;
}

/** Test pointer to RefCountedObj type. */
AFL_TEST("afl.base.Ref:base", a) {
    a.checkEqual("01. live", RefCountedObj::live, 0);

    {
        afl::base::Ref<RefCountedObj> pi(*new RefCountedObj(12));
        a.checkEqual("11. live", RefCountedObj::live, 1);
        a.checkEqual("12. value", pi->n, 12);

        afl::base::Ref<RefCountedObj> pj(pi);
        a.checkEqual("21. live", RefCountedObj::live, 1);
        a.checkEqual("22. value", pj->n, 12);

        pi.reset(*new RefCountedObj(42));
        a.checkEqual("31. live", RefCountedObj::live, 2);
        a.checkEqual("32. value", pi->n, 42);
        a.checkEqual("33. value", pj->n, 12);

        pj.reset(*pi);
        a.checkEqual("41. live", RefCountedObj::live, 1);
        a.checkEqual("42. value", pi->n, 42);
        a.checkEqual("43. value", pj->n, 42);
    }

    a.checkEqual("99. live", RefCountedObj::live, 0);
}

/*
 *  Test derivation
 */

namespace {
    class Derived1 : public RefCountedObj { };
    class Derived2 : public RefCountedObj { };
}

/** Test working with a class hierarchy. */
AFL_TEST("afl.base.Ref:derived", a) {
    a.checkEqual("01", RefCountedObj::live, 0);

    // Make Derived1
    afl::base::Ref<Derived1> pd1(*new Derived1());
    a.checkEqual("02", RefCountedObj::live, 1);

    // Make Base
    afl::base::Ref<RefCountedObj> pb(*new RefCountedObj());
    a.checkEqual("03", RefCountedObj::live, 2);

    // Store Derived1 in RefCountedObj reference
    afl::base::Ref<RefCountedObj> pb2(pd1);
    a.checkEqual("04", RefCountedObj::live, 2);

    // Assign Derived1 to Base (Base dies)
    pb.reset(*pd1);
    a.checkEqual("05", RefCountedObj::live, 1);

    // Make Derived2
    afl::base::Ref<Derived2> pd2(*new Derived2());
    a.checkEqual("06", RefCountedObj::live, 2);
}

/** Test interaction with Ptr. */
AFL_TEST("afl.base.Ref:ptr", a) {
    a.checkEqual("01. live", RefCountedObj::live, 0);

    {
        afl::base::Ref<RefCountedObj> pi(*new RefCountedObj(42));
        a.checkEqual("11. live", RefCountedObj::live, 1);
        a.checkEqual("12. value", pi->n, 42);

        // Convert to Ptr
        afl::base::Ptr<RefCountedObj> pp1(pi.asPtr());
        afl::base::Ptr<RefCountedObj> pp2(&*pi);
        a.checkEqual("21. live", RefCountedObj::live, 1);
        a.checkEqual("22. addr", &*pi, &*pp1);
        a.checkEqual("23. addr", &*pi, &*pp2);

        pp1 = 0;
        pp2 = 0;
        a.checkEqual("31. live", RefCountedObj::live, 1);
        a.checkEqual("32. value", pi->n, 42);
    }
    a.checkEqual("99. live", RefCountedObj::live, 0);
}

/** Test use of const objects. */
AFL_TEST("afl.base.Ref:const", a) {
    a.checkEqual("01. live", RefCountedObj::live, 0);
    {
        const RefCountedObj* p = new RefCountedObj(12);
        afl::base::Ref<const RefCountedObj> pi(*p);
        a.checkEqual("11. live", RefCountedObj::live, 1);
        a.checkEqual("12. value", pi->n, 12);
    }
    a.checkEqual("99. live", RefCountedObj::live, 0);
}
