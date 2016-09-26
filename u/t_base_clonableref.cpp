/**
  *  \file u/t_base_clonableref.cpp
  *  \brief Test for afl::base::ClonableRef
  */

#include "afl/base/clonableref.hpp"

#include "u/t_base.hpp"

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

/** Test suite. */
void
TestBaseClonableRef::testIt()
{
    TS_ASSERT_EQUALS(Tester::s_count, 0);

    {
        // Allocate one
        afl::base::ClonableRef<Tester> t(new Tester(42));
        TS_ASSERT_EQUALS(Tester::s_count, 1);
        TS_ASSERT_EQUALS(t->val(), 42);

        // Allocate another
        afl::base::ClonableRef<Tester> t2(new Tester(23));
        TS_ASSERT_EQUALS(Tester::s_count, 2);
        TS_ASSERT_EQUALS(t2->val(), 23);

        // Cloning
        afl::base::ClonableRef<Tester> t3(t2);
        TS_ASSERT_EQUALS(Tester::s_count, 3);
        TS_ASSERT_EQUALS(t3->val(), 23);

        // Assignment
        t2 = t;
        TS_ASSERT_EQUALS(Tester::s_count, 3);
        TS_ASSERT_EQUALS(t->val(),  42);
        TS_ASSERT_EQUALS(t2->val(), 42);
        TS_ASSERT_EQUALS(t3->val(), 23);

        // Assignment to clone
        t2 = t;
        TS_ASSERT_EQUALS(Tester::s_count, 3);
        TS_ASSERT_EQUALS(t->val(),  42);
        TS_ASSERT_EQUALS(t2->val(), 42);
        TS_ASSERT_EQUALS(t3->val(), 23);

        // Self-assignment
        t2 = t2;
        TS_ASSERT_EQUALS(Tester::s_count, 3);
        TS_ASSERT_EQUALS(t->val(),  42);
        TS_ASSERT_EQUALS(t2->val(), 42);
        TS_ASSERT_EQUALS(t3->val(), 23);

        // Also test operator*
        TS_ASSERT_EQUALS((*t).val(), 42);
    }

    TS_ASSERT_EQUALS(Tester::s_count, 0);
}
