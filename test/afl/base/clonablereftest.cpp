/**
  *  \file test/afl/base/clonablereftest.cpp
  *  \brief Test for afl::base::ClonableRef
  */

#include "afl/base/clonableref.hpp"
#include "afl/test/testrunner.hpp"

namespace {
    class Tester {
     public:
        Tester(int n)
            : m_val(n)
            { ++s_count; }
        ~Tester()
            { --s_count; }

        Tester* clone() const
            { return new Tester(m_val); }

        int val() const
            { return m_val; }

        static int s_count;
     private:
        Tester(const Tester&);
        Tester& operator=(Tester&);

        int m_val;
    };

    int Tester::s_count;
}

AFL_TEST("afl.base.ClonableRef", a) {
    a.checkEqual("initial state must be 0", Tester::s_count, 0);

    {
        // Allocate one
        afl::base::ClonableRef<Tester> t(new Tester(42));
        a.checkEqual("must have one object", Tester::s_count, 1);
        a.checkEqual("first object must have correct value", t->val(), 42);

        // Allocate another
        afl::base::ClonableRef<Tester> t2(new Tester(23));
        a.checkEqual("must have two objects", Tester::s_count, 2);
        a.checkEqual("second object must have correct value", t2->val(), 23);

        // Cloning
        afl::base::ClonableRef<Tester> t3(t2);
        a.checkEqual("must have three objects", Tester::s_count, 3);
        a.checkEqual("cloned object must have correct value", t3->val(), 23);

        // Assignment
        t2 = t;
        a.checkEqual("must have three objects after assignment", Tester::s_count, 3);
        a.checkEqual("first value after assignment", t->val(),  42);
        a.checkEqual("second value after assignment", t2->val(), 42);
        a.checkEqual("third value after assignment", t3->val(), 23);

        // Assignment to clone
        t2 = t;
        a.checkEqual("must have three objects after clone", Tester::s_count, 3);
        a.checkEqual("first value after clone", t->val(),  42);
        a.checkEqual("second value after clone", t2->val(), 42);
        a.checkEqual("third value after clone", t3->val(), 23);

        // Self-assignment
        t2 = t2;
        a.checkEqual("must have three objects after self-assignment", Tester::s_count, 3);
        a.checkEqual("first value after self-assignment", t->val(),  42);
        a.checkEqual("second value after self-assignment", t2->val(), 42);
        a.checkEqual("third value after self-assignment", t3->val(), 23);

        // Also test operator*
        a.checkEqual("value using operator*", (*t).val(), 42);
    }

    a.checkEqual("final state must be 0", Tester::s_count, 0);
}
