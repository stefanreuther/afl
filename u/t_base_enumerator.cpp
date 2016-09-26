/**
  *  \file u/t_base_enumerator.cpp
  *  \brief Test for afl::base::Enumerator
  */

#include "afl/base/enumerator.hpp"

#include "u/t_base.hpp"

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
void
TestBaseEnumerator::testIt()
{
    Tester t;

    int i;
    TS_ASSERT(t.getNextElement(i));
    TS_ASSERT_EQUALS(i, 0);

    int j;
    while (t.getNextElement(j)) {
        i += j;
    }
    TS_ASSERT_EQUALS(i, 45);

    // Make sure it's stuck
    TS_ASSERT_EQUALS(t.getNextElement(j), false);
    TS_ASSERT_EQUALS(t.getNextElement(j), false);
    TS_ASSERT_EQUALS(t.getNextElement(j), false);
}
