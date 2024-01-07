/**
  *  \file test/afl/container/dereferencingiteratortest.cpp
  *  \brief Test for afl::container::DereferencingIterator
  */

#include "afl/container/dereferencingiterator.hpp"

#include <vector>
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.container.DereferencingIterator", a)
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
    a.checkEqual("idx 0", *i, 1);  a.check("idx 0 end", i != ite(pvec.end())); ++i;
    a.checkEqual("idx 1+0", i[0], 2);
    a.checkEqual("idx 1+1", i[1], 3);
    a.checkEqual("idx 1", *i, 2);  a.check("idx 1 end", i != ite(pvec.end())); ++i;
    a.checkEqual("idx 2", *i, 3);  a.check("idx 2 end", i != ite(pvec.end())); ++i;
    a.checkEqual("idx 3", *i, 17); a.check("idx 3 end", i != ite(pvec.end())); i += 1;
    a.checkEqual("idx 4", *i, 42); a.check("idx 4 end", i != ite(pvec.end())); i++;
    a.checkEqual("idx 5", *i, -1); a.check("idx 5 end", i != ite(pvec.end())); ++i;
    a.check("idx 6 end", i == ite(pvec.end()));

    for (std::vector<int*>::iterator p = pvec.begin(); p != pvec.end(); ++p) {
        delete *p;
        *p = 0;
    }
}
