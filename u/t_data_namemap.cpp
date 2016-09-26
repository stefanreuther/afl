/**
  *  \file u/t_data_namemap.cpp
  *  \brief Test for afl::io::NameMap
  */

#include "afl/data/namemap.hpp"

#include "u/t_data.hpp"

/** Simple tests. */
void
TestDataNameMap::testIt()
{
    afl::data::NameMap names;
    TS_ASSERT_EQUALS(names.getNumNames(), 0U);
    TS_ASSERT_EQUALS(names.getIndexByName("a"), names.nil);

    TS_ASSERT_EQUALS(names.add("a"), 0U);
    TS_ASSERT_EQUALS(names.add("b"), 1U);
    TS_ASSERT_EQUALS(names.add("c"), 2U);

    TS_ASSERT_EQUALS(names.getIndexByName("a"), 0U);
    TS_ASSERT_EQUALS(names.getIndexByName("b"), 1U);
    TS_ASSERT_EQUALS(names.getIndexByName("c"), 2U);

    TS_ASSERT_EQUALS(names.getNameByIndex(0), "a");
    TS_ASSERT_EQUALS(names.getNameByIndex(1), "b");
    TS_ASSERT_EQUALS(names.getNameByIndex(2), "c");

    TS_ASSERT_EQUALS(names.addMaybe("a"), 0U);
    TS_ASSERT_EQUALS(names.addMaybe("b"), 1U);
    TS_ASSERT_EQUALS(names.addMaybe("c"), 2U);

    TS_ASSERT_EQUALS(names.add("a"), 3U);
    TS_ASSERT_EQUALS(names.getIndexByName("a"), 3U);
    TS_ASSERT_EQUALS(names.addMaybe("a"), 3U);
    TS_ASSERT_EQUALS(names.getNameByIndex(3), "a");
    TS_ASSERT_EQUALS(names.getNameByIndex(0), "a");
}
