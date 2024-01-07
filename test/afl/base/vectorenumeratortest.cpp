/**
  *  \file test/afl/base/vectorenumeratortest.cpp
  *  \brief Test for afl::base::VectorEnumerator
  */

#include "afl/base/vectorenumerator.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.base.VectorEnumerator:init", a)
{
    std::vector<int> is;
    is.push_back(3);
    is.push_back(1);
    is.push_back(4);
    afl::base::VectorEnumerator<int> testee(is);

    int out = 0;
    a.check("get first", testee.getNextElement(out));
    a.checkEqual("first value", out, 3);
    a.check("get second", testee.getNextElement(out));
    a.checkEqual("second value", out, 1);
    a.check("get third", testee.getNextElement(out));
    a.checkEqual("third value", out, 4);
    a.check("final", !testee.getNextElement(out));
}

AFL_TEST("afl.base.VectorEnumerator:add", a)
{
    afl::base::VectorEnumerator<int> testee;
    testee.add(9);
    testee.add(6);
    testee.add(5);

    int out = 0;
    a.check("get first", testee.getNextElement(out));
    a.checkEqual("first value", out, 9);
    a.check("get second", testee.getNextElement(out));
    a.checkEqual("second value", out, 6);
    a.check("get third", testee.getNextElement(out));
    a.checkEqual("third value", out, 5);
    a.check("final", !testee.getNextElement(out));
}
