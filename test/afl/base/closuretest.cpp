/**
  *  \file test/afl/base/closuretest.cpp
  *  \brief Test for afl::base::Closure
  */

#include "afl/base/closure.hpp"
#include "afl/test/testrunner.hpp"

namespace {
    /*
     *  Object methods
     */
    class Tester {
     public:
        void nullary()
            { m_val = 0; }
        void unary(int a)
            { m_val = a; }
        void binary(int a, int b)
            { m_val = a + b; }
        void ternary(int a, int b, int c)
            { m_val = a + b + c; }
        void quaternary(int a, int b, int c, int d)
            { m_val = a + b + c + d; }
        void quinary(int a, int b, int c, int d, int e)
            { m_val = a + b + c + d + e; }
        void senary(int a, int b, int c, int d, int e, int f)
            { m_val = a + b + c + d + e + f; }

        void set(int n)
            { m_val = n; }
        int get() const
            { return m_val; }

     private:
        int m_val;
    };

    /*
     *  Static methods
     */
    int g_val;

    void nullary()
        { g_val = 0; }
    void unary(int a)
        { g_val = a; }
    void binary(int a, int b)
        { g_val = a + b; }
    void ternary(int a, int b, int c)
        { g_val = a + b + c; }
    void quaternary(int a, int b, int c, int d)
        { g_val = a + b + c + d; }
    void quinary(int a, int b, int c, int d, int e)
        { g_val = a + b + c + d + e; }
    void senary(int a, int b, int c, int d, int e, int f)
        { g_val = a + b + c + d + e + f; }

    /*
     *  Smart pointer tests
     */
    class Pointer {
     public:
        Pointer(Tester& t)
            : m_t(t)
            { }

        Tester& operator*()
            { return m_t; }

     private:
        Tester& m_t;
    };
}

/*
 *  Regular construction
 */

/** Test nullary closures. */
AFL_TEST("afl.base.Closure:nullary", a) {
    // Make static
    std::auto_ptr<afl::base::Closure<void()> > cc(new afl::base::Closure<void()>::Static(nullary));
    g_val = 1;
    a.checkEqual("before static", g_val, 1);
    cc->call();
    a.checkEqual("after static", g_val, 0);

    // Object method
    Tester t;
    cc.reset(new afl::base::Closure<void()>::Bound<Tester>(&t, &Tester::nullary));
    t.set(1);
    a.checkEqual("before method", t.get(), 1);
    cc->call();
    a.checkEqual("after method", t.get(), 0);
}

/** Test unary closures. */
AFL_TEST("afl.base.Closure:unary", a) {
    typedef afl::base::Closure<void(int)> Closure;

    // Make static
    std::auto_ptr<Closure> cc(new Closure::Static(unary));
    g_val = 1;
    a.checkEqual("before static", g_val, 1);
    cc->call(2);
    a.checkEqual("after static", g_val, 2);

    // Object method
    Tester t;
    cc.reset(new Closure::Bound<Tester>(&t, &Tester::unary));
    t.set(1);
    a.checkEqual("before method", t.get(), 1);
    cc->call(4);
    a.checkEqual("after method", t.get(), 4);

    // Nullary function
    cc.reset(new Closure::StaticNullary(nullary));
    cc->call(99);
    a.checkEqual("after nullary static", g_val, 0);

    // Nullary member
    cc.reset(new Closure::BoundNullary<Tester>(&t, &Tester::nullary));
    cc->call(77);
    a.checkEqual("after nullary method", t.get(), 0);
}

/** Test binary closures. */
AFL_TEST("afl.base.Closure:binary", a) {
    typedef afl::base::Closure<void(int,int)> Closure;

    // Make static
    std::auto_ptr<Closure> cc(new Closure::Static(binary));
    g_val = 1;
    a.checkEqual("before static", g_val, 1);
    cc->call(2, 3);
    a.checkEqual("after static", g_val, 5);

    // Object method
    Tester t;
    cc.reset(new Closure::Bound<Tester>(&t, &Tester::binary));
    t.set(1);
    a.checkEqual("before method", t.get(), 1);
    cc->call(4, 5);
    a.checkEqual("after method", t.get(), 9);

    // Nullary function
    cc.reset(new Closure::StaticNullary(nullary));
    cc->call(99, 98);
    a.checkEqual("after nullary static", g_val, 0);

    // Nullary member
    cc.reset(new Closure::BoundNullary<Tester>(&t, &Tester::nullary));
    cc->call(77, 76);
    a.checkEqual("after nullary method", t.get(), 0);
}

/** Test ternary closures. */
AFL_TEST("afl.base.Closure:ternary", a) {
    typedef afl::base::Closure<void(int,int,int)> Closure;

    // Make static
    std::auto_ptr<Closure> cc(new Closure::Static(ternary));
    g_val = 1;
    a.checkEqual("before static", g_val, 1);
    cc->call(2, 3, 4);
    a.checkEqual("after static", g_val, 9);

    // Object method
    Tester t;
    cc.reset(new Closure::Bound<Tester>(&t, &Tester::ternary));
    t.set(1);
    a.checkEqual("before method", t.get(), 1);
    cc->call(4, 5, 6);
    a.checkEqual("after method", t.get(), 15);

    // Nullary function
    cc.reset(new Closure::StaticNullary(nullary));
    cc->call(99, 98, 97);
    a.checkEqual("after nullary static", g_val, 0);

    // Nullary member
    cc.reset(new Closure::BoundNullary<Tester>(&t, &Tester::nullary));
    cc->call(77, 76, 75);
    a.checkEqual("after nullary method", t.get(), 0);
}

/** Test quaternary closures. */
AFL_TEST("afl.base.Closure:quaternary", a) {
    typedef afl::base::Closure<void(int,int,int,int)> Closure;

    // Make static
    std::auto_ptr<Closure> cc(new Closure::Static(quaternary));
    g_val = 1;
    a.checkEqual("before static", g_val, 1);
    cc->call(2, 3, 4, 5);
    a.checkEqual("after static", g_val, 14);

    // Object method
    Tester t;
    cc.reset(new Closure::Bound<Tester>(&t, &Tester::quaternary));
    t.set(1);
    a.checkEqual("before method", t.get(), 1);
    cc->call(4, 5, 6, 7);
    a.checkEqual("after method", t.get(), 22);

    // Nullary function
    cc.reset(new Closure::StaticNullary(nullary));
    cc->call(99, 98, 97, 96);
    a.checkEqual("after nullary static", g_val, 0);

    // Nullary member
    cc.reset(new Closure::BoundNullary<Tester>(&t, &Tester::nullary));
    cc->call(77, 76, 75, 74);
    a.checkEqual("after nullary method", t.get(), 0);
}

/** Test quinary closures. */
AFL_TEST("afl.base.Closure:quinary", a) {
    typedef afl::base::Closure<void(int,int,int,int,int)> Closure;

    // Make static
    std::auto_ptr<Closure> cc(new Closure::Static(quinary));
    g_val = 1;
    a.checkEqual("before static", g_val, 1);
    cc->call(2, 3, 4, 5, 6);
    a.checkEqual("after static", g_val, 20);

    // Object method
    Tester t;
    cc.reset(new Closure::Bound<Tester>(&t, &Tester::quinary));
    t.set(1);
    a.checkEqual("before method", t.get(), 1);
    cc->call(4, 5, 6, 7, 8);
    a.checkEqual("after method", t.get(), 30);

    // Nullary function
    cc.reset(new Closure::StaticNullary(nullary));
    cc->call(99, 98, 97, 96, 95);
    a.checkEqual("after nullary static", g_val, 0);

    // Nullary member
    cc.reset(new Closure::BoundNullary<Tester>(&t, &Tester::nullary));
    cc->call(77, 76, 75, 74, 73);
    a.checkEqual("after nullary method", t.get(), 0);
}

/** Test senary closures. */
AFL_TEST("afl.base.Closure:senary", a) {
    typedef afl::base::Closure<void(int,int,int,int,int,int)> Closure;

    // Make static
    std::auto_ptr<Closure> cc(new Closure::Static(senary));
    g_val = 1;
    a.checkEqual("before static", g_val, 1);
    cc->call(2, 3, 4, 5, 6, 7);
    a.checkEqual("after static", g_val, 27);

    // Object method
    Tester t;
    cc.reset(new Closure::Bound<Tester>(&t, &Tester::senary));
    t.set(1);
    a.checkEqual("before method", t.get(), 1);
    cc->call(4, 5, 6, 7, 8, 9);
    a.checkEqual("after method", t.get(), 39);

    // Nullary function
    cc.reset(new Closure::StaticNullary(nullary));
    cc->call(99, 98, 97, 96, 95, 94);
    a.checkEqual("after nullary static", g_val, 0);

    // Nullary member
    cc.reset(new Closure::BoundNullary<Tester>(&t, &Tester::nullary));
    cc->call(77, 76, 75, 74, 73, 72);
    a.checkEqual("after nullary method", t.get(), 0);
}

/*
 *  Same thing, now using factory methods
 */

/** Test nullary closure factory methods. */
AFL_TEST("afl.base.Closure:nullary-factory", a) {
    // Make static
    std::auto_ptr<afl::base::Closure<void()> > cc(afl::base::Closure<void()>::makeStatic(nullary));
    g_val = 1;
    a.checkEqual("before static", g_val, 1);
    cc->call();
    a.checkEqual("after static", g_val, 0);

    // Object method
    Tester t;
    cc.reset(afl::base::Closure<void()>::makeBound(&t, &Tester::nullary));
    t.set(1);
    a.checkEqual("before method", t.get(), 1);
    cc->call();
    a.checkEqual("after method", t.get(), 0);
}

/** Test unary closure factory methods. */
AFL_TEST("afl.base.Closure:unary-factory", a) {
    typedef afl::base::Closure<void(int)> Closure;

    // Make static
    std::auto_ptr<Closure> cc(Closure::makeStatic(unary));
    g_val = 1;
    a.checkEqual("before static", g_val, 1);
    cc->call(2);
    a.checkEqual("after static", g_val, 2);

    // Object method
    Tester t;
    cc.reset(Closure::makeBound(&t, &Tester::unary));
    t.set(1);
    a.checkEqual("before method", t.get(), 1);
    cc->call(4);
    a.checkEqual("after method", t.get(), 4);

    // Nullary function
    cc.reset(Closure::makeStatic(nullary));
    cc->call(99);
    a.checkEqual("after nullary static", g_val, 0);

    // Null
    cc.reset(Closure::makeNull());
    cc->call(99);
    a.checkEqual("after null", g_val, 0);

    // Nullary member
    cc.reset(Closure::makeBound(&t, &Tester::nullary));
    cc->call(77);
    a.checkEqual("after nullary method", t.get(), 0);
}

/** Test binary closure factory methods. */
AFL_TEST("afl.base.Closure:binary-factory", a) {
    typedef afl::base::Closure<void(int,int)> Closure;

    // Make static
    std::auto_ptr<Closure> cc(Closure::makeStatic(binary));
    g_val = 1;
    a.checkEqual("before static", g_val, 1);
    cc->call(2, 3);
    a.checkEqual("after static", g_val, 5);

    // Object method
    Tester t;
    cc.reset(Closure::makeBound(&t, &Tester::binary));
    t.set(1);
    a.checkEqual("before method", t.get(), 1);
    cc->call(4, 5);
    a.checkEqual("after method", t.get(), 9);

    // Nullary function
    cc.reset(Closure::makeStatic(nullary));
    cc->call(99, 98);
    a.checkEqual("after nullary static", g_val, 0);

    // Null
    cc.reset(Closure::makeNull());
    cc->call(99, 98);
    a.checkEqual("after null", g_val, 0);

    // Nullary member
    cc.reset(Closure::makeBound(&t, &Tester::nullary));
    cc->call(77, 76);
    a.checkEqual("after nullary method", t.get(), 0);
}

/** Test ternary closure factory methods. */
AFL_TEST("afl.base.Closure:ternary-factory", a) {
    typedef afl::base::Closure<void(int,int,int)> Closure;

    // Make static
    std::auto_ptr<Closure> cc(Closure::makeStatic(ternary));
    g_val = 1;
    a.checkEqual("before static", g_val, 1);
    cc->call(2, 3, 4);
    a.checkEqual("after static", g_val, 9);

    // Object method
    Tester t;
    cc.reset(Closure::makeBound(&t, &Tester::ternary));
    t.set(1);
    a.checkEqual("before method", t.get(), 1);
    cc->call(4, 5, 6);
    a.checkEqual("after method", t.get(), 15);

    // Nullary function
    cc.reset(Closure::makeStatic(nullary));
    cc->call(99, 98, 97);
    a.checkEqual("after nullary static", g_val, 0);

    // Null
    cc.reset(Closure::makeNull());
    cc->call(99, 98, 97);
    a.checkEqual("after null", g_val, 0);

    // Nullary member
    cc.reset(Closure::makeBound(&t, &Tester::nullary));
    cc->call(77, 76, 75);
    a.checkEqual("after nullary method", t.get(), 0);
}

/** Test quaternary closure factory methods. */
AFL_TEST("afl.base.Closure:quaternary-factory", a) {
    typedef afl::base::Closure<void(int,int,int,int)> Closure;

    // Make static
    std::auto_ptr<Closure> cc(Closure::makeStatic(quaternary));
    g_val = 1;
    a.checkEqual("before static", g_val, 1);
    cc->call(2, 3, 4, 5);
    a.checkEqual("after static", g_val, 14);

    // Object method
    Tester t;
    cc.reset(Closure::makeBound(&t, &Tester::quaternary));
    t.set(1);
    a.checkEqual("before method", t.get(), 1);
    cc->call(4, 5, 6, 7);
    a.checkEqual("after method", t.get(), 22);

    // Nullary function
    cc.reset(Closure::makeStatic(nullary));
    cc->call(99, 98, 97, 96);
    a.checkEqual("after nullary static", g_val, 0);

    // Null
    cc.reset(Closure::makeNull());
    cc->call(99, 98, 97, 96);
    a.checkEqual("after null", g_val, 0);

    // Nullary member
    cc.reset(Closure::makeBound(&t, &Tester::nullary));
    cc->call(77, 76, 75, 74);
    a.checkEqual("after nullary method", t.get(), 0);
}

/** Test quinary closure factory methods. */
AFL_TEST("afl.base.Closure:quinary-factory", a) {
    typedef afl::base::Closure<void(int,int,int,int,int)> Closure;

    // Make static
    std::auto_ptr<Closure> cc(Closure::makeStatic(quinary));
    g_val = 1;
    a.checkEqual("before static", g_val, 1);
    cc->call(2, 3, 4, 5, 6);
    a.checkEqual("after static", g_val, 20);

    // Object method
    Tester t;
    cc.reset(Closure::makeBound(&t, &Tester::quinary));
    t.set(1);
    a.checkEqual("before method", t.get(), 1);
    cc->call(4, 5, 6, 7, 8);
    a.checkEqual("after method", t.get(), 30);

    // Nullary function
    cc.reset(Closure::makeStatic(nullary));
    cc->call(99, 98, 97, 96, 95);
    a.checkEqual("after nullary static", g_val, 0);

    // Null
    cc.reset(Closure::makeNull());
    cc->call(99, 98, 97, 96, 95);
    a.checkEqual("after null", g_val, 0);

    // Nullary member
    cc.reset(Closure::makeBound(&t, &Tester::nullary));
    cc->call(77, 76, 75, 74, 73);
    a.checkEqual("after nullary method", t.get(), 0);
}

/** Test senary closure factory methods. */
AFL_TEST("afl.base.Closure:senary-factory", a) {
    typedef afl::base::Closure<void(int,int,int,int,int,int)> Closure;

    // Make static
    std::auto_ptr<Closure> cc(Closure::makeStatic(senary));
    g_val = 1;
    a.checkEqual("before static", g_val, 1);
    cc->call(2, 3, 4, 5, 6, 7);
    a.checkEqual("after static", g_val, 27);

    // Object method
    Tester t;
    cc.reset(Closure::makeBound(&t, &Tester::senary));
    t.set(1);
    a.checkEqual("before method", t.get(), 1);
    cc->call(4, 5, 6, 7, 8, 9);
    a.checkEqual("after method", t.get(), 39);

    // Nullary function
    cc.reset(Closure::makeStatic(nullary));
    cc->call(99, 98, 97, 96, 95, 94);
    a.checkEqual("after nullary static", g_val, 0);

    // Null
    cc.reset(Closure::makeNull());
    cc->call(99, 98, 97, 96, 95, 94);
    a.checkEqual("after null", g_val, 0);

    // Nullary member
    cc.reset(Closure::makeBound(&t, &Tester::nullary));
    cc->call(77, 76, 75, 74, 73, 72);
    a.checkEqual("after nullary method", t.get(), 0);
}

/** Test interaction between closures and smart pointers. */
AFL_TEST("afl.base.Closure:smart", a) {
    Tester t;
    Pointer p(t);

    // Initialize
    t.set(42);
    a.checkEqual("before", t.get(), 42);

    // Make closure
    typedef afl::base::Closure<void(int)> Closure;
    std::auto_ptr<Closure> cc(Closure::makeBound(p, &Tester::unary));

    // Test it
    cc->call(9);
    a.checkEqual("after", t.get(), 9);
}

/** Test null closures. */
AFL_TEST("afl.base.Closure:null", a) {
    // With void return type
    afl::base::Closure<void()>* p = afl::base::Closure<void()>::makeNull();
    p->call();
    delete p;

    // With integer return type
    afl::base::Closure<int()>* p2 = afl::base::Closure<int()>::makeNull();
    int result = p2->call();
    delete p2;
    a.checkEqual("result", result, 0);
}
