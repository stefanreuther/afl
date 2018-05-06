/**
  *  \file u/t_base_ptr.cpp
  *  \brief Test for afl::base::Ptr
  */

#include "afl/base/ptr.hpp"

#include "u/t_base.hpp"

/*
 *  Pointer to fundamental type
 */

/** Test pointer to fundamental type. */
void
TestBasePtr::testInt()
{
    afl::base::Ptr<int> pi = new int(17);
    afl::base::Ptr<int> pj;
    pj = pi;
    pi = new int(23);

    TS_ASSERT_EQUALS(*pi, 23);
    TS_ASSERT_EQUALS(*pj, 17);

    TS_ASSERT_EQUALS(*pi.get(), 23);
}

/*
 *  Pointer to object type
 */

namespace {
    class Obj {
        Obj(const Obj&);
        Obj& operator=(const Obj&);
     public:
        int n;
        static int live;
        Obj(int n)
            : n(n) { ++live; }
        ~Obj()
            { --live; }
    };
    int Obj::live = 0;
}

/** Test pointer to object type. */
void
TestBasePtr::testObj()
{
    TS_ASSERT_EQUALS(Obj::live, 0);

    afl::base::Ptr<Obj> pi = new Obj(12);
    TS_ASSERT_EQUALS(Obj::live, 1);
    TS_ASSERT_EQUALS(pi->n, 12);

    afl::base::Ptr<Obj> pj(pi);
    TS_ASSERT_EQUALS(Obj::live, 1);
    TS_ASSERT_EQUALS(pj->n, 12);
    TS_ASSERT_EQUALS(pi.get(), pj.get());

    pi.reset();
    TS_ASSERT_EQUALS(pi.get(), (Obj*) 0);

    TS_ASSERT_EQUALS(Obj::live, 1);
    TS_ASSERT_EQUALS(pj->n, 12);

    pi = new Obj(42);
    TS_ASSERT_EQUALS(Obj::live, 2);
    TS_ASSERT_EQUALS(pi->n, 42);
    TS_ASSERT_EQUALS(pj->n, 12);

    pj = pi;
    TS_ASSERT_EQUALS(Obj::live, 1);
    TS_ASSERT_EQUALS(pi->n, 42);
    TS_ASSERT_EQUALS(pj->n, 42);

    // Pass through dumb pointer
    pi = 0;
    pi = pj.get();
    TS_ASSERT_EQUALS(Obj::live, 1);
    TS_ASSERT_EQUALS(pi->n, 42);
    TS_ASSERT_EQUALS(pj->n, 42);

    pi = 0;
    pj = 0;
    TS_ASSERT_EQUALS(Obj::live, 0);
}

/** Test pointer to const object type. */
void
TestBasePtr::testConstObj()
{
    TS_ASSERT_EQUALS(Obj::live, 0);
    {
        const Obj* p = new Obj(77);
        afl::base::Ptr<const Obj> pi = p;
        TS_ASSERT_EQUALS(Obj::live, 1);
        TS_ASSERT_EQUALS(pi->n, 77);
    }
    TS_ASSERT_EQUALS(Obj::live, 0);
}

/*
 *  Pointer to RefCounted object type
 */

namespace {
    class RefCountedObj {
        RefCountedObj(const RefCountedObj&);
        RefCountedObj& operator=(const RefCountedObj&);
     public:
        int n;
        static int live;
        RefCountedObj(int n)
            : n(n) { ++live; }
        ~RefCountedObj()
            { --live; }
    };
    int RefCountedObj::live = 0;
}

/** Test pointer to RefCounted type. */
void
TestBasePtr::testRefCountedObj()
{
    TS_ASSERT_EQUALS(RefCountedObj::live, 0);

    afl::base::Ptr<RefCountedObj> pi = new RefCountedObj(12);
    TS_ASSERT_EQUALS(RefCountedObj::live, 1);
    TS_ASSERT_EQUALS(pi->n, 12);

    afl::base::Ptr<RefCountedObj> pj(pi);
    TS_ASSERT_EQUALS(RefCountedObj::live, 1);
    TS_ASSERT_EQUALS(pj->n, 12);
    TS_ASSERT_EQUALS(pi.get(), pj.get());

    pi.reset();
    TS_ASSERT_EQUALS(pi.get(), (RefCountedObj*) 0);

    TS_ASSERT_EQUALS(RefCountedObj::live, 1);
    TS_ASSERT_EQUALS(pj->n, 12);

    pi = new RefCountedObj(42);
    TS_ASSERT_EQUALS(RefCountedObj::live, 2);
    TS_ASSERT_EQUALS(pi->n, 42);
    TS_ASSERT_EQUALS(pj->n, 12);

    pj = pi;
    TS_ASSERT_EQUALS(RefCountedObj::live, 1);
    TS_ASSERT_EQUALS(pi->n, 42);
    TS_ASSERT_EQUALS(pj->n, 42);

    // Pass through dumb pointer
    pi = 0;
    pi = pj.get();
    TS_ASSERT_EQUALS(RefCountedObj::live, 1);
    TS_ASSERT_EQUALS(pi->n, 42);
    TS_ASSERT_EQUALS(pj->n, 42);

    pi = 0;
    pj = 0;
    TS_ASSERT_EQUALS(RefCountedObj::live, 0);
}

/*
 *  Test derivation
 */

namespace {
    class Base {
     public:
        Base()
            : m_obj(1)
            { }
        virtual ~Base() { }
     private:
        Obj m_obj;
    };
    class Derived1 : public Base { };
    class Derived2 : public Base { };
}

/** Test pointer to derived classes. */
void
TestBasePtr::testDerivation()
{
    TS_ASSERT_EQUALS(Obj::live, 0);

    // Make Derived1
    afl::base::Ptr<Derived1> pd1(new Derived1());
    TS_ASSERT_EQUALS(Obj::live, 1);

    // Make Base
    afl::base::Ptr<Base> pb(new Base());
    TS_ASSERT_EQUALS(Obj::live, 2);

    // Store Derived1 in Base pointer
    afl::base::Ptr<Base> pb2(pd1);
    TS_ASSERT_EQUALS(Obj::live, 2);

    // Assign Derived1 to Base (Base dies)
    pb = pd1;
    TS_ASSERT_EQUALS(Obj::live, 1);

    // Make Derived2
    afl::base::Ptr<Derived2> pd2(new Derived2());
    TS_ASSERT_EQUALS(Obj::live, 2);

    // Try casting Derived1->Derived1
    afl::base::Ptr<Derived1> pd11 = pd1.cast<Derived1>();
    TS_ASSERT_EQUALS(pd11.get(), pd1.get());

    // Try casting Base->Derived1. Base still contains the Derived1 object.
    pd11 = pb.cast<Derived1>();
    TS_ASSERT_EQUALS(pd11.get(), pd1.get());

    // Same thing, now with Derived2
    pb = pd2;
    pd11 = pb.cast<Derived1>();
    TS_ASSERT_EQUALS(pd11.get(), (Derived1*) 0);
}

namespace {
    class CopyDestructor {
     public:
        CopyDestructor(afl::base::Ptr<CopyDestructor>& ref)
            : m_ref(ref)
            { }
        ~CopyDestructor()
            {
                afl::base::Ptr<CopyDestructor> copy = m_ref;
            }
     private:
        afl::base::Ptr<CopyDestructor>& m_ref;
    };
}

/** Test copying the pointer in the destructor.
    The wrong implementation of the destructor (decRef) will cause infinite recursion.

    See https://www.reddit.com/r/cpp/comments/79ak5d/a_bug_in_stdshared_ptr/ */
void
TestBasePtr::testCopyDestructor()
{
    // Variant 1
    {
        afl::base::Ptr<CopyDestructor> p(new CopyDestructor(p));
    }

    // Variant 2
    {
        afl::base::Ptr<CopyDestructor> p(new CopyDestructor(p));
        p.reset();
    }
}

namespace {
    class AssignDestructor {
     public:
        AssignDestructor(afl::base::Ptr<AssignDestructor>* p)
            : m_p(p)
            { }
        ~AssignDestructor()
            {
                if (m_p) {
                    *m_p = new AssignDestructor(0);
                }
            }
     private:
        afl::base::Ptr<AssignDestructor>* m_p;
    };
}

/** Test re-assigning the pointer in the destructor.
    This is a variation of testCopyDestructor(). */
void
TestBasePtr::testAssignDestructor()
{
    // Variant 1
    {
        afl::base::Ptr<AssignDestructor> p(new AssignDestructor(&p));
    }

    // Variant 2
    {
        afl::base::Ptr<AssignDestructor> p(new AssignDestructor(&p));
        p.reset();
    }
}

