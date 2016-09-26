/**
  *  \file u/t_base_clonable.cpp
  *  \brief Test for afl::base::Clonable
  */

#include "afl/base/clonable.hpp"

#include "u/t_base.hpp"

namespace {
    class Testee : public afl::base::Clonable<Testee> {
     public:
        Testee(int n)
            : m_n(n)
            { }

        Testee* clone() const
            { return new Testee(m_n); }

        int getN() const
            { return m_n; }

     private:
        int m_n;
    };
}

/** Simple test. */
void
TestBaseClonable::testIt()
{
    // Stack testee
    Testee t(42);
    TS_ASSERT_EQUALS(t.getN(), 42);

    // We can clone stack objects
    Testee* p = t.clone();
    TS_ASSERT_DIFFERS(p, &t);
    TS_ASSERT_EQUALS(p->getN(), 42);

    // We can clone heap objects
    Testee* q = p->clone();
    TS_ASSERT_DIFFERS(p, q);
    TS_ASSERT_EQUALS(q->getN(), 42);

    // cloneOf
    Testee* r = Testee::cloneOf(p);
    TS_ASSERT_DIFFERS(p, r);
    TS_ASSERT_EQUALS(r->getN(), 42);

    TS_ASSERT_EQUALS(Testee::cloneOf(0), (Testee*) 0);

    delete p;
    delete q;
    delete r;
}
