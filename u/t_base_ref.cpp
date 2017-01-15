/**
  *  \file u/t_base_ref.cpp
  *  \brief Test for afl::base::Ref
  */

#include "afl/base/ref.hpp"

#include "t_base.hpp"

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
void
TestBaseRef::testObj()
{
    TS_ASSERT_EQUALS(RefCountedObj::live, 0);

    {
        afl::base::Ref<RefCountedObj> pi(*new RefCountedObj(12));
        TS_ASSERT_EQUALS(RefCountedObj::live, 1);
        TS_ASSERT_EQUALS(pi->n, 12);

        afl::base::Ref<RefCountedObj> pj(pi);
        TS_ASSERT_EQUALS(RefCountedObj::live, 1);
        TS_ASSERT_EQUALS(pj->n, 12);

        pi.reset(*new RefCountedObj(42));
        TS_ASSERT_EQUALS(RefCountedObj::live, 2);
        TS_ASSERT_EQUALS(pi->n, 42);
        TS_ASSERT_EQUALS(pj->n, 12);

        pj.reset(*pi);
        TS_ASSERT_EQUALS(RefCountedObj::live, 1);
        TS_ASSERT_EQUALS(pi->n, 42);
        TS_ASSERT_EQUALS(pj->n, 42);
    }

    TS_ASSERT_EQUALS(RefCountedObj::live, 0);
}

/*
 *  Test derivation
 */

namespace {
    class Derived1 : public RefCountedObj { };
    class Derived2 : public RefCountedObj { };
}

/** Test working with a class hierarchy. */
void
TestBaseRef::testDerived()
{
    TS_ASSERT_EQUALS(RefCountedObj::live, 0);

    // Make Derived1
    afl::base::Ref<Derived1> pd1(*new Derived1());
    TS_ASSERT_EQUALS(RefCountedObj::live, 1);

    // Make Base
    afl::base::Ref<RefCountedObj> pb(*new RefCountedObj());
    TS_ASSERT_EQUALS(RefCountedObj::live, 2);

    // Store Derived1 in RefCountedObj reference
    afl::base::Ref<RefCountedObj> pb2(pd1);
    TS_ASSERT_EQUALS(RefCountedObj::live, 2);

    // Assign Derived1 to Base (Base dies)
    pb.reset(*pd1);
    TS_ASSERT_EQUALS(RefCountedObj::live, 1);

    // Make Derived2
    afl::base::Ref<Derived2> pd2(*new Derived2());
    TS_ASSERT_EQUALS(RefCountedObj::live, 2);
}

/** Test interaction with Ptr. */
void
TestBaseRef::testPtr()
{
    TS_ASSERT_EQUALS(RefCountedObj::live, 0);

    {
        afl::base::Ref<RefCountedObj> pi(*new RefCountedObj(42));
        TS_ASSERT_EQUALS(RefCountedObj::live, 1);
        TS_ASSERT_EQUALS(pi->n, 42);

        // Convert to Ptr
        afl::base::Ptr<RefCountedObj> pp1(pi.asPtr());
        afl::base::Ptr<RefCountedObj> pp2(&*pi);
        TS_ASSERT_EQUALS(RefCountedObj::live, 1);
        TS_ASSERT_EQUALS(&*pi, &*pp1);
        TS_ASSERT_EQUALS(&*pi, &*pp2);

        pp1 = 0;
        pp2 = 0;
        TS_ASSERT_EQUALS(RefCountedObj::live, 1);
        TS_ASSERT_EQUALS(pi->n, 42);
    }
    TS_ASSERT_EQUALS(RefCountedObj::live, 0);
}
