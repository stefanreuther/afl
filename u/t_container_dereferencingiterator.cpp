/**
  *  \file u/t_container_dereferencingiterator.cpp
  *  \brief Test for afl::container::DereferencingIterator
  */

#include "afl/container/dereferencingiterator.hpp"

#include <vector>
#include "u/t_container.hpp"

/** Simple test. */
void
TestContainerDereferencingIterator::testIt()
{
    std::vector<int*> pvec;
    pvec.push_back(new int(1));
    pvec.push_back(new int(2));
    pvec.push_back(new int(3));
    pvec.push_back(new int(17));
    pvec.push_back(new int(42));
    pvec.push_back(new int(-1));

    typedef afl::container::DereferencingIterator<std::vector<int*>::iterator> ite;

    ite i = ite(pvec.begin());
    TS_ASSERT_EQUALS(*i, 1);  TS_ASSERT_DIFFERS(i, ite(pvec.end())); ++i;
    TS_ASSERT_EQUALS(i[0], 2);
    TS_ASSERT_EQUALS(i[1], 3);
    TS_ASSERT_EQUALS(*i, 2);  TS_ASSERT_DIFFERS(i, ite(pvec.end())); ++i;
    TS_ASSERT_EQUALS(*i, 3);  TS_ASSERT_DIFFERS(i, ite(pvec.end())); ++i;
    TS_ASSERT_EQUALS(*i, 17); TS_ASSERT_DIFFERS(i, ite(pvec.end())); i += 1;
    TS_ASSERT_EQUALS(*i, 42); TS_ASSERT_DIFFERS(i, ite(pvec.end())); i++;
    TS_ASSERT_EQUALS(*i, -1); TS_ASSERT_DIFFERS(i, ite(pvec.end())); ++i;
    TS_ASSERT_EQUALS(i, ite(pvec.end()));

    for (std::vector<int*>::iterator p = pvec.begin(); p != pvec.end(); ++p) {
        delete *p;
        *p = 0;
    }
}
