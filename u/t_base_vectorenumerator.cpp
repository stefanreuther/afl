/**
  *  \file u/t_base_vectorenumerator.cpp
  *  \brief Test for afl::base::VectorEnumerator
  */

#include "afl/base/vectorenumerator.hpp"

#include "t_base.hpp"

void
TestBaseVectorEnumerator::testInit()
{
    std::vector<int> is;
    is.push_back(3);
    is.push_back(1);
    is.push_back(4);
    afl::base::VectorEnumerator<int> testee(is);

    int out = 0;
    TS_ASSERT(testee.getNextElement(out));
    TS_ASSERT_EQUALS(out, 3);
    TS_ASSERT(testee.getNextElement(out));
    TS_ASSERT_EQUALS(out, 1);
    TS_ASSERT(testee.getNextElement(out));
    TS_ASSERT_EQUALS(out, 4);
    TS_ASSERT(!testee.getNextElement(out));
}

void
TestBaseVectorEnumerator::testAdd()
{
    afl::base::VectorEnumerator<int> testee;
    testee.add(9);
    testee.add(6);
    testee.add(5);

    int out = 0;
    TS_ASSERT(testee.getNextElement(out));
    TS_ASSERT_EQUALS(out, 9);
    TS_ASSERT(testee.getNextElement(out));
    TS_ASSERT_EQUALS(out, 6);
    TS_ASSERT(testee.getNextElement(out));
    TS_ASSERT_EQUALS(out, 5);
    TS_ASSERT(!testee.getNextElement(out));
}

