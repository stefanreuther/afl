/**
  *  \file test/afl/data/integerlisttest.cpp
  *  \brief Test for afl::data::IntegerList
  */

#include "afl/data/integerlist.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.data.IntegerList", a)
{
    afl::data::IntegerList_t obj;
    obj.push_back(10);
    obj.push_back(20);
    a.checkEqual("size", obj.size(), 2U);
    a.checkEqual("value", obj[0], 10);
}
