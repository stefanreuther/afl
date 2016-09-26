/**
  *  \file u/t_base_closure.cpp
  *  \brief Test for afl::base::Closure etc.
  */

#include <memory>
#include "afl/base/closure.hpp"

#include "u/t_base.hpp"

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
void
TestBaseClosure::testNullary()
{
    // Make static
    std::auto_ptr<afl::base::Closure<void()> > cc(new afl::base::Closure<void()>::Static(nullary));
    g_val = 1;
    TS_ASSERT_EQUALS(g_val, 1);
    cc->call();
    TS_ASSERT_EQUALS(g_val, 0);

    // Clone it
    std::auto_ptr<afl::base::Closure<void()> > cc2(cc->clone());
    g_val = 1;
    TS_ASSERT_EQUALS(g_val, 1);
    cc2->call();
    TS_ASSERT_EQUALS(g_val, 0);

    // Object method
    Tester t;
    cc.reset(new afl::base::Closure<void()>::Bound<Tester>(&t, &Tester::nullary));
    t.set(1);
    TS_ASSERT_EQUALS(t.get(), 1);
    cc->call();
    TS_ASSERT_EQUALS(t.get(), 0);
    delete cc->clone();

    // Clone is unaffected
    g_val = 1;
    TS_ASSERT_EQUALS(g_val, 1);
    cc2->call();
    TS_ASSERT_EQUALS(g_val, 0);
}

/** Test unary closures. */
void
TestBaseClosure::testUnary()
{
    typedef afl::base::Closure<void(int)> Closure;

    // Make static
    std::auto_ptr<Closure> cc(new Closure::Static(unary));
    g_val = 1;
    TS_ASSERT_EQUALS(g_val, 1);
    cc->call(2);
    TS_ASSERT_EQUALS(g_val, 2);

    // Clone it
    std::auto_ptr<Closure> cc2(cc->clone());
    cc2->call(3);
    TS_ASSERT_EQUALS(g_val, 3);

    // Object method
    Tester t;
    cc.reset(new Closure::Bound<Tester>(&t, &Tester::unary));
    t.set(1);
    TS_ASSERT_EQUALS(t.get(), 1);
    cc->call(4);
    TS_ASSERT_EQUALS(t.get(), 4);
    delete cc->clone();

    // Clone is unaffected
    cc2->call(7);
    TS_ASSERT_EQUALS(g_val, 7);
    TS_ASSERT_EQUALS(t.get(), 4);

    // Nullary function
    cc.reset(new Closure::StaticNullary(nullary));
    cc->call(99);
    TS_ASSERT_EQUALS(g_val, 0);
    delete cc->clone();

    // Nullary member
    cc.reset(new Closure::BoundNullary<Tester>(&t, &Tester::nullary));
    cc->call(77);
    TS_ASSERT_EQUALS(t.get(), 0);
    delete cc->clone();
}

/** Test binary closures. */
void
TestBaseClosure::testBinary()
{
    typedef afl::base::Closure<void(int,int)> Closure;

    // Make static
    std::auto_ptr<Closure> cc(new Closure::Static(binary));
    g_val = 1;
    TS_ASSERT_EQUALS(g_val, 1);
    cc->call(2, 3);
    TS_ASSERT_EQUALS(g_val, 5);

    // Clone it
    std::auto_ptr<Closure> cc2(cc->clone());
    cc2->call(3, 4);
    TS_ASSERT_EQUALS(g_val, 7);

    // Object method
    Tester t;
    cc.reset(new Closure::Bound<Tester>(&t, &Tester::binary));
    t.set(1);
    TS_ASSERT_EQUALS(t.get(), 1);
    cc->call(4, 5);
    TS_ASSERT_EQUALS(t.get(), 9);
    delete cc->clone();

    // Clone is unaffected
    cc2->call(7, 8);
    TS_ASSERT_EQUALS(g_val, 15);
    TS_ASSERT_EQUALS(t.get(), 9);

    // Nullary function
    cc.reset(new Closure::StaticNullary(nullary));
    cc->call(99, 98);
    TS_ASSERT_EQUALS(g_val, 0);
    delete cc->clone();

    // Nullary member
    cc.reset(new Closure::BoundNullary<Tester>(&t, &Tester::nullary));
    cc->call(77, 76);
    TS_ASSERT_EQUALS(t.get(), 0);
    delete cc->clone();
}

/** Test ternary closures. */
void
TestBaseClosure::testTernary()
{
    typedef afl::base::Closure<void(int,int,int)> Closure;

    // Make static
    std::auto_ptr<Closure> cc(new Closure::Static(ternary));
    g_val = 1;
    TS_ASSERT_EQUALS(g_val, 1);
    cc->call(2, 3, 4);
    TS_ASSERT_EQUALS(g_val, 9);

    // Clone it
    std::auto_ptr<Closure> cc2(cc->clone());
    cc2->call(3, 4, 5);
    TS_ASSERT_EQUALS(g_val, 12);

    // Object method
    Tester t;
    cc.reset(new Closure::Bound<Tester>(&t, &Tester::ternary));
    t.set(1);
    TS_ASSERT_EQUALS(t.get(), 1);
    cc->call(4, 5, 6);
    TS_ASSERT_EQUALS(t.get(), 15);
    delete cc->clone();

    // Clone is unaffected
    cc2->call(7, 8, 9);
    TS_ASSERT_EQUALS(g_val, 24);
    TS_ASSERT_EQUALS(t.get(), 15);

    // Nullary function
    cc.reset(new Closure::StaticNullary(nullary));
    cc->call(99, 98, 97);
    TS_ASSERT_EQUALS(g_val, 0);
    delete cc->clone();

    // Nullary member
    cc.reset(new Closure::BoundNullary<Tester>(&t, &Tester::nullary));
    cc->call(77, 76, 75);
    TS_ASSERT_EQUALS(t.get(), 0);
    delete cc->clone();
}

/** Test quaternary closures. */
void
TestBaseClosure::testQuaternary()
{
    typedef afl::base::Closure<void(int,int,int,int)> Closure;

    // Make static
    std::auto_ptr<Closure> cc(new Closure::Static(quaternary));
    g_val = 1;
    TS_ASSERT_EQUALS(g_val, 1);
    cc->call(2, 3, 4, 5);
    TS_ASSERT_EQUALS(g_val, 14);

    // Clone it
    std::auto_ptr<Closure> cc2(cc->clone());
    cc2->call(3, 4, 5, 6);
    TS_ASSERT_EQUALS(g_val, 18);

    // Object method
    Tester t;
    cc.reset(new Closure::Bound<Tester>(&t, &Tester::quaternary));
    t.set(1);
    TS_ASSERT_EQUALS(t.get(), 1);
    cc->call(4, 5, 6, 7);
    TS_ASSERT_EQUALS(t.get(), 22);
    delete cc->clone();

    // Clone is unaffected
    cc2->call(7, 8, 9, 10);
    TS_ASSERT_EQUALS(g_val, 34);
    TS_ASSERT_EQUALS(t.get(), 22);

    // Nullary function
    cc.reset(new Closure::StaticNullary(nullary));
    cc->call(99, 98, 97, 96);
    TS_ASSERT_EQUALS(g_val, 0);
    delete cc->clone();

    // Nullary member
    cc.reset(new Closure::BoundNullary<Tester>(&t, &Tester::nullary));
    cc->call(77, 76, 75, 74);
    TS_ASSERT_EQUALS(t.get(), 0);
    delete cc->clone();
}

/** Test quinary closures. */
void
TestBaseClosure::testQuinary()
{
    typedef afl::base::Closure<void(int,int,int,int,int)> Closure;

    // Make static
    std::auto_ptr<Closure> cc(new Closure::Static(quinary));
    g_val = 1;
    TS_ASSERT_EQUALS(g_val, 1);
    cc->call(2, 3, 4, 5, 6);
    TS_ASSERT_EQUALS(g_val, 20);

    // Clone it
    std::auto_ptr<Closure> cc2(cc->clone());
    cc2->call(3, 4, 5, 6, 7);
    TS_ASSERT_EQUALS(g_val, 25);

    // Object method
    Tester t;
    cc.reset(new Closure::Bound<Tester>(&t, &Tester::quinary));
    t.set(1);
    TS_ASSERT_EQUALS(t.get(), 1);
    cc->call(4, 5, 6, 7, 8);
    TS_ASSERT_EQUALS(t.get(), 30);
    delete cc->clone();

    // Clone is unaffected
    cc2->call(7, 8, 9, 10, 11);
    TS_ASSERT_EQUALS(g_val, 45);
    TS_ASSERT_EQUALS(t.get(), 30);

    // Nullary function
    cc.reset(new Closure::StaticNullary(nullary));
    cc->call(99, 98, 97, 96, 95);
    TS_ASSERT_EQUALS(g_val, 0);
    delete cc->clone();

    // Nullary member
    cc.reset(new Closure::BoundNullary<Tester>(&t, &Tester::nullary));
    cc->call(77, 76, 75, 74, 73);
    TS_ASSERT_EQUALS(t.get(), 0);
    delete cc->clone();
}

/** Test senary closures. */
void
TestBaseClosure::testSenary()
{
    typedef afl::base::Closure<void(int,int,int,int,int,int)> Closure;

    // Make static
    std::auto_ptr<Closure> cc(new Closure::Static(senary));
    g_val = 1;
    TS_ASSERT_EQUALS(g_val, 1);
    cc->call(2, 3, 4, 5, 6, 7);
    TS_ASSERT_EQUALS(g_val, 27);

    // Clone it
    std::auto_ptr<Closure> cc2(cc->clone());
    cc2->call(3, 4, 5, 6, 7, 8);
    TS_ASSERT_EQUALS(g_val, 33);

    // Object method
    Tester t;
    cc.reset(new Closure::Bound<Tester>(&t, &Tester::senary));
    t.set(1);
    TS_ASSERT_EQUALS(t.get(), 1);
    cc->call(4, 5, 6, 7, 8, 9);
    TS_ASSERT_EQUALS(t.get(), 39);
    delete cc->clone();

    // Clone is unaffected
    cc2->call(7, 8, 9, 10, 11, 12);
    TS_ASSERT_EQUALS(g_val, 57);
    TS_ASSERT_EQUALS(t.get(), 39);

    // Nullary function
    cc.reset(new Closure::StaticNullary(nullary));
    cc->call(99, 98, 97, 96, 95, 94);
    TS_ASSERT_EQUALS(g_val, 0);
    delete cc->clone();

    // Nullary member
    cc.reset(new Closure::BoundNullary<Tester>(&t, &Tester::nullary));
    cc->call(77, 76, 75, 74, 73, 72);
    TS_ASSERT_EQUALS(t.get(), 0);
    delete cc->clone();
}

/*
 *  Same thing, now using factory methods
 */

/** Test nullary closure factory methods. */
void
TestBaseClosure::testNullaryFactory()
{
    // Make static
    std::auto_ptr<afl::base::Closure<void()> > cc(afl::base::Closure<void()>::makeStatic(nullary));
    g_val = 1;
    TS_ASSERT_EQUALS(g_val, 1);
    cc->call();
    TS_ASSERT_EQUALS(g_val, 0);

    // Clone it
    std::auto_ptr<afl::base::Closure<void()> > cc2(cc->clone());
    g_val = 1;
    TS_ASSERT_EQUALS(g_val, 1);
    cc2->call();
    TS_ASSERT_EQUALS(g_val, 0);

    // Object method
    Tester t;
    cc.reset(afl::base::Closure<void()>::makeBound(&t, &Tester::nullary));
    t.set(1);
    TS_ASSERT_EQUALS(t.get(), 1);
    cc->call();
    TS_ASSERT_EQUALS(t.get(), 0);
    delete cc->clone();

    // Clone is unaffected
    g_val = 1;
    TS_ASSERT_EQUALS(g_val, 1);
    cc2->call();
    TS_ASSERT_EQUALS(g_val, 0);
}

/** Test unary closure factory methods. */
void
TestBaseClosure::testUnaryFactory()
{
    typedef afl::base::Closure<void(int)> Closure;

    // Make static
    std::auto_ptr<Closure> cc(Closure::makeStatic(unary));
    g_val = 1;
    TS_ASSERT_EQUALS(g_val, 1);
    cc->call(2);
    TS_ASSERT_EQUALS(g_val, 2);

    // Clone it
    std::auto_ptr<Closure> cc2(cc->clone());
    cc2->call(3);
    TS_ASSERT_EQUALS(g_val, 3);

    // Object method
    Tester t;
    cc.reset(Closure::makeBound(&t, &Tester::unary));
    t.set(1);
    TS_ASSERT_EQUALS(t.get(), 1);
    cc->call(4);
    TS_ASSERT_EQUALS(t.get(), 4);
    delete cc->clone();

    // Clone is unaffected
    cc2->call(7);
    TS_ASSERT_EQUALS(g_val, 7);
    TS_ASSERT_EQUALS(t.get(), 4);

    // Nullary function
    cc.reset(Closure::makeStatic(nullary));
    cc->call(99);
    TS_ASSERT_EQUALS(g_val, 0);
    delete cc->clone();

    // Nullary member
    cc.reset(Closure::makeBound(&t, &Tester::nullary));
    cc->call(77);
    TS_ASSERT_EQUALS(t.get(), 0);
    delete cc->clone();
}

/** Test binary closure factory methods. */
void
TestBaseClosure::testBinaryFactory()
{
    typedef afl::base::Closure<void(int,int)> Closure;

    // Make static
    std::auto_ptr<Closure> cc(Closure::makeStatic(binary));
    g_val = 1;
    TS_ASSERT_EQUALS(g_val, 1);
    cc->call(2, 3);
    TS_ASSERT_EQUALS(g_val, 5);

    // Clone it
    std::auto_ptr<Closure> cc2(cc->clone());
    cc2->call(3, 4);
    TS_ASSERT_EQUALS(g_val, 7);

    // Object method
    Tester t;
    cc.reset(Closure::makeBound(&t, &Tester::binary));
    t.set(1);
    TS_ASSERT_EQUALS(t.get(), 1);
    cc->call(4, 5);
    TS_ASSERT_EQUALS(t.get(), 9);
    delete cc->clone();

    // Clone is unaffected
    cc2->call(7, 8);
    TS_ASSERT_EQUALS(g_val, 15);
    TS_ASSERT_EQUALS(t.get(), 9);

    // Nullary function
    cc.reset(Closure::makeStatic(nullary));
    cc->call(99, 98);
    TS_ASSERT_EQUALS(g_val, 0);
    delete cc->clone();

    // Nullary member
    cc.reset(Closure::makeBound(&t, &Tester::nullary));
    cc->call(77, 76);
    TS_ASSERT_EQUALS(t.get(), 0);
    delete cc->clone();
}

/** Test ternary closure factory methods. */
void
TestBaseClosure::testTernaryFactory()
{
    typedef afl::base::Closure<void(int,int,int)> Closure;

    // Make static
    std::auto_ptr<Closure> cc(Closure::makeStatic(ternary));
    g_val = 1;
    TS_ASSERT_EQUALS(g_val, 1);
    cc->call(2, 3, 4);
    TS_ASSERT_EQUALS(g_val, 9);

    // Clone it
    std::auto_ptr<Closure> cc2(cc->clone());
    cc2->call(3, 4, 5);
    TS_ASSERT_EQUALS(g_val, 12);

    // Object method
    Tester t;
    cc.reset(Closure::makeBound(&t, &Tester::ternary));
    t.set(1);
    TS_ASSERT_EQUALS(t.get(), 1);
    cc->call(4, 5, 6);
    TS_ASSERT_EQUALS(t.get(), 15);
    delete cc->clone();

    // Clone is unaffected
    cc2->call(7, 8, 9);
    TS_ASSERT_EQUALS(g_val, 24);
    TS_ASSERT_EQUALS(t.get(), 15);

    // Nullary function
    cc.reset(Closure::makeStatic(nullary));
    cc->call(99, 98, 97);
    TS_ASSERT_EQUALS(g_val, 0);
    delete cc->clone();

    // Nullary member
    cc.reset(Closure::makeBound(&t, &Tester::nullary));
    cc->call(77, 76, 75);
    TS_ASSERT_EQUALS(t.get(), 0);
    delete cc->clone();
}

/** Test quaternary closure factory methods. */
void
TestBaseClosure::testQuaternaryFactory()
{
    typedef afl::base::Closure<void(int,int,int,int)> Closure;

    // Make static
    std::auto_ptr<Closure> cc(Closure::makeStatic(quaternary));
    g_val = 1;
    TS_ASSERT_EQUALS(g_val, 1);
    cc->call(2, 3, 4, 5);
    TS_ASSERT_EQUALS(g_val, 14);

    // Clone it
    std::auto_ptr<Closure> cc2(cc->clone());
    cc2->call(3, 4, 5, 6);
    TS_ASSERT_EQUALS(g_val, 18);

    // Object method
    Tester t;
    cc.reset(Closure::makeBound(&t, &Tester::quaternary));
    t.set(1);
    TS_ASSERT_EQUALS(t.get(), 1);
    cc->call(4, 5, 6, 7);
    TS_ASSERT_EQUALS(t.get(), 22);
    delete cc->clone();

    // Clone is unaffected
    cc2->call(7, 8, 9, 10);
    TS_ASSERT_EQUALS(g_val, 34);
    TS_ASSERT_EQUALS(t.get(), 22);

    // Nullary function
    cc.reset(Closure::makeStatic(nullary));
    cc->call(99, 98, 97, 96);
    TS_ASSERT_EQUALS(g_val, 0);
    delete cc->clone();

    // Nullary member
    cc.reset(Closure::makeBound(&t, &Tester::nullary));
    cc->call(77, 76, 75, 74);
    TS_ASSERT_EQUALS(t.get(), 0);
    delete cc->clone();
}

/** Test quinary closure factory methods. */
void
TestBaseClosure::testQuinaryFactory()
{
    typedef afl::base::Closure<void(int,int,int,int,int)> Closure;

    // Make static
    std::auto_ptr<Closure> cc(Closure::makeStatic(quinary));
    g_val = 1;
    TS_ASSERT_EQUALS(g_val, 1);
    cc->call(2, 3, 4, 5, 6);
    TS_ASSERT_EQUALS(g_val, 20);

    // Clone it
    std::auto_ptr<Closure> cc2(cc->clone());
    cc2->call(3, 4, 5, 6, 7);
    TS_ASSERT_EQUALS(g_val, 25);

    // Object method
    Tester t;
    cc.reset(Closure::makeBound(&t, &Tester::quinary));
    t.set(1);
    TS_ASSERT_EQUALS(t.get(), 1);
    cc->call(4, 5, 6, 7, 8);
    TS_ASSERT_EQUALS(t.get(), 30);
    delete cc->clone();

    // Clone is unaffected
    cc2->call(7, 8, 9, 10, 11);
    TS_ASSERT_EQUALS(g_val, 45);
    TS_ASSERT_EQUALS(t.get(), 30);

    // Nullary function
    cc.reset(Closure::makeStatic(nullary));
    cc->call(99, 98, 97, 96, 95);
    TS_ASSERT_EQUALS(g_val, 0);
    delete cc->clone();

    // Nullary member
    cc.reset(Closure::makeBound(&t, &Tester::nullary));
    cc->call(77, 76, 75, 74, 73);
    TS_ASSERT_EQUALS(t.get(), 0);
    delete cc->clone();
}

/** Test senary closure factory methods. */
void
TestBaseClosure::testSenaryFactory()
{
    typedef afl::base::Closure<void(int,int,int,int,int,int)> Closure;

    // Make static
    std::auto_ptr<Closure> cc(Closure::makeStatic(senary));
    g_val = 1;
    TS_ASSERT_EQUALS(g_val, 1);
    cc->call(2, 3, 4, 5, 6, 7);
    TS_ASSERT_EQUALS(g_val, 27);

    // Clone it
    std::auto_ptr<Closure> cc2(cc->clone());
    cc2->call(3, 4, 5, 6, 7, 8);
    TS_ASSERT_EQUALS(g_val, 33);

    // Object method
    Tester t;
    cc.reset(Closure::makeBound(&t, &Tester::senary));
    t.set(1);
    TS_ASSERT_EQUALS(t.get(), 1);
    cc->call(4, 5, 6, 7, 8, 9);
    TS_ASSERT_EQUALS(t.get(), 39);
    delete cc->clone();

    // Clone is unaffected
    cc2->call(7, 8, 9, 10, 11, 12);
    TS_ASSERT_EQUALS(g_val, 57);
    TS_ASSERT_EQUALS(t.get(), 39);

    // Nullary function
    cc.reset(Closure::makeStatic(nullary));
    cc->call(99, 98, 97, 96, 95, 94);
    TS_ASSERT_EQUALS(g_val, 0);
    delete cc->clone();

    // Nullary member
    cc.reset(Closure::makeBound(&t, &Tester::nullary));
    cc->call(77, 76, 75, 74, 73, 72);
    TS_ASSERT_EQUALS(t.get(), 0);
    delete cc->clone();
}

/** Test interaction between closures and smart pointers. */
void
TestBaseClosure::testSmart()
{
    Tester t;
    Pointer p(t);

    // Initialize
    t.set(42);
    TS_ASSERT_EQUALS(t.get(), 42);

    // Make closure
    typedef afl::base::Closure<void(int)> Closure;
    std::auto_ptr<Closure> cc(Closure::makeBound(p, &Tester::unary));

    // Test it
    cc->call(9);
    TS_ASSERT_EQUALS(t.get(), 9);
}

/** Test null closures. */
void
TestBaseClosure::testNull()
{
    // With void return type
    afl::base::Closure<void()>* p = afl::base::Closure<void()>::makeNull();
    p->call();
    delete p;

    // With integer return type
    afl::base::Closure<int()>* p2 = afl::base::Closure<int()>::makeNull();
    int result = p2->call();
    delete p2;
    TS_ASSERT_EQUALS(result, 0);
}
