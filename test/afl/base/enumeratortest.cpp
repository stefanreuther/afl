/**
  *  \file test/afl/base/enumeratortest.cpp
  *  \brief Test for afl::base::Enumerator
  */

#include "afl/base/enumerator.hpp"
#include "afl/test/testrunner.hpp"

namespace {
    class Tester : public afl::base::Enumerator<int> {
     public:
        Tester()
            : m_val(0)
            { }

        bool getNextElement(int& result)
            {
                if (m_val < 10) {
                    result = m_val++;
                    return true;
                } else {
                    return false;
                }
            }

     private:
        int m_val;
    };
}

/** Test simple enumeration.
    Since this is an abstract base class, we cannot test much more than that the header file is well-formed,
    i.e. allows us to derive a class that works the way it's meant to be. */
AFL_TEST("afl.base.Enumerator", a) {
    Tester t;

    int i;
    a.check("first element retrievable", t.getNextElement(i));
    a.checkEqual("first element value", i, 0);

    int j;
    while (t.getNextElement(j)) {
        i += j;
    }
    a.checkEqual("total elements", i, 45);

    // Make sure it's stuck
    a.checkEqual("end 1", t.getNextElement(j), false);
    a.checkEqual("end 2", t.getNextElement(j), false);
    a.checkEqual("end 3", t.getNextElement(j), false);
}
