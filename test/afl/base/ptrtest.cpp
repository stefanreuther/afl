/**
  *  \file test/afl/base/ptrtest.cpp
  *  \brief Test for afl::base::Ptr
  */

#include "afl/base/ptr.hpp"
#include "afl/test/testrunner.hpp"
#include "afl/base/refcounted.hpp"

/*
 *  Pointer to fundamental type
 */

/** Test pointer to fundamental type. */
AFL_TEST("afl.base.Ptr:int", a)
{
    afl::base::Ptr<int> pi = new int(17);
    afl::base::Ptr<int> pj;
    pj = pi;
    pi = new int(23);

    a.checkEqual("*pi", *pi, 23);
    a.checkEqual("*pj", *pj, 17);

    a.checkEqual("*pi.get", *pi.get(), 23);
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
AFL_TEST("afl.base.Ptr:obj", a)
{
    a.checkEqual("01. live", Obj::live, 0);

    afl::base::Ptr<Obj> pi = new Obj(12);
    a.checkEqual("11. live", Obj::live, 1);
    a.checkEqual("12. value", pi->n, 12);

    afl::base::Ptr<Obj> pj(pi);
    a.checkEqual("21. live", Obj::live, 1);
    a.checkEqual("22. value", pj->n, 12);
    a.checkEqual("23. get", pi.get(), pj.get());

    pi.reset();
    a.checkEqual("31. get", pi.get(), (Obj*) 0);
    a.checkEqual("32. live", Obj::live, 1);
    a.checkEqual("33. value", pj->n, 12);

    pi = new Obj(42);
    a.checkEqual("41. live", Obj::live, 2);
    a.checkEqual("42. value", pi->n, 42);
    a.checkEqual("43. value", pj->n, 12);

    pj = pi;
    a.checkEqual("51. live", Obj::live, 1);
    a.checkEqual("52. value", pi->n, 42);
    a.checkEqual("53. value", pj->n, 42);

    // Pass through dumb pointer
    pi = 0;
    pi = pj.get();
    a.checkEqual("61. live", Obj::live, 1);
    a.checkEqual("62. value", pi->n, 42);
    a.checkEqual("63. value", pj->n, 42);

    pi = 0;
    pj = 0;
    a.checkEqual("71. live", Obj::live, 0);
}

/** Test pointer to const object type. */
AFL_TEST("afl.base.Ptr:const", a) {
    a.checkEqual("live before", Obj::live, 0);
    {
        const Obj* p = new Obj(77);
        afl::base::Ptr<const Obj> pi = p;
        a.checkEqual("live", Obj::live, 1);
        a.checkEqual("value", pi->n, 77);
    }
    a.checkEqual("live after", Obj::live, 0);
}

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
        RefCountedObj(int n)
            : n(n) { ++live; }
        ~RefCountedObj()
            { --live; }
    };
    int RefCountedObj::live = 0;
}

/** Test pointer to RefCounted type. */
AFL_TEST("afl.base.Ptr:RefCounted", a) {
    a.checkEqual("01. live", RefCountedObj::live, 0);

    afl::base::Ptr<RefCountedObj> pi = new RefCountedObj(12);
    a.checkEqual("11. live", RefCountedObj::live, 1);
    a.checkEqual("12. value", pi->n, 12);

    afl::base::Ptr<RefCountedObj> pj(pi);
    a.checkEqual("21. live", RefCountedObj::live, 1);
    a.checkEqual("22. value", pj->n, 12);
    a.checkEqual("23. get", pi.get(), pj.get());

    pi.reset();
    a.checkEqual("31. get", pi.get(), (RefCountedObj*) 0);
    a.checkEqual("32. live", RefCountedObj::live, 1);
    a.checkEqual("33. value", pj->n, 12);

    pi = new RefCountedObj(42);
    a.checkEqual("41. live", RefCountedObj::live, 2);
    a.checkEqual("42. value", pi->n, 42);
    a.checkEqual("43. value", pj->n, 12);

    pj = pi;
    a.checkEqual("51. live", RefCountedObj::live, 1);
    a.checkEqual("52. value", pi->n, 42);
    a.checkEqual("53. value", pj->n, 42);

    // Pass through dumb pointer
    pi = 0;
    pi = pj.get();
    a.checkEqual("61. live", RefCountedObj::live, 1);
    a.checkEqual("62. value", pi->n, 42);
    a.checkEqual("63. value", pj->n, 42);

    pi = 0;
    pj = 0;
    a.checkEqual("71. live", RefCountedObj::live, 0);
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
AFL_TEST("afl.base.Ptr:derived-class", a) {
    a.checkEqual("01", Obj::live, 0);

    // Make Derived1
    afl::base::Ptr<Derived1> pd1(new Derived1());
    a.checkEqual("02", Obj::live, 1);

    // Make Base
    afl::base::Ptr<Base> pb(new Base());
    a.checkEqual("03", Obj::live, 2);

    // Store Derived1 in Base pointer
    afl::base::Ptr<Base> pb2(pd1);
    a.checkEqual("04", Obj::live, 2);

    // Assign Derived1 to Base (Base dies)
    pb = pd1;
    a.checkEqual("05", Obj::live, 1);

    // Make Derived2
    afl::base::Ptr<Derived2> pd2(new Derived2());
    a.checkEqual("06", Obj::live, 2);

    // Try casting Derived1->Derived1
    afl::base::Ptr<Derived1> pd11 = pd1.cast<Derived1>();
    a.checkEqual("07", pd11.get(), pd1.get());

    // Try casting Base->Derived1. Base still contains the Derived1 object.
    pd11 = pb.cast<Derived1>();
    a.checkEqual("08", pd11.get(), pd1.get());

    // Same thing, now with Derived2
    pb = pd2;
    pd11 = pb.cast<Derived1>();
    a.checkEqual("09", pd11.get(), (Derived1*) 0);
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
AFL_TEST_NOARG("afl.base.Ptr:copy-destructor:1") {
    // Variant 1
    afl::base::Ptr<CopyDestructor> p(new CopyDestructor(p));
}
AFL_TEST_NOARG("afl.base.Ptr:copy-destructor:2") {
    // Variant 2
    afl::base::Ptr<CopyDestructor> p(new CopyDestructor(p));
    p.reset();
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
AFL_TEST_NOARG("afl.base.Ptr:assign-destructor:1") {
    // Variant 1
    afl::base::Ptr<AssignDestructor> p(new AssignDestructor(&p));
}
AFL_TEST_NOARG("afl.base.Ptr:assign-destructor:2") {
    // Variant 2
    afl::base::Ptr<AssignDestructor> p(new AssignDestructor(&p));
    p.reset();
}
