/**
  *  \file u/t_data_integerlist.cpp
  *  \brief Test for afl::data::IntegerList_t
  */

#include "afl/data/integerlist.hpp"

#include "u/t_data.hpp"

/** Simple test. */
void
TestDataIntegerList::testIt()
{
    afl::data::IntegerList_t obj;
    obj.push_back(10);
    obj.push_back(20);
    TS_ASSERT_EQUALS(obj.size(), 2U);
    TS_ASSERT_EQUALS(obj[0], 10);
}
