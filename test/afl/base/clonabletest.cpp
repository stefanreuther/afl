/**
  *  \file test/afl/base/clonabletest.cpp
  *  \brief test for afl::base::Clonable
  */

#include "afl/base/clonable.hpp"
#include "afl/test/testrunner.hpp"

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
AFL_TEST("afl.base.Clonable", a) {
    // Stack testee
    Testee t(42);
    a.checkEqual("original value", t.getN(), 42);

    // We can clone stack objects
    Testee* p = t.clone();
    a.checkDifferent("first clone", p, &t);
    a.checkEqual("first clone value", p->getN(), 42);

    // We can clone heap objects
    Testee* q = p->clone();
    a.checkDifferent("second clone", p, q);
    a.checkEqual("second clone value", q->getN(), 42);

    // cloneOf
    Testee* r = Testee::cloneOf(p);
    a.checkDifferent("third clone", p, r);
    a.checkEqual("third clone value", r->getN(), 42);

    a.checkNull("clone of null", Testee::cloneOf(0));

    delete p;
    delete q;
    delete r;
}
