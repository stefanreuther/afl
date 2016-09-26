/**
  *  \file u/t_data_stringlist.cpp
  *  \brief Test for afl::data::StringList_t
  */

#include "afl/data/stringlist.hpp"

#include "u/t_data.hpp"

/** Simple test. */
void
TestDataStringList::testIt()
{
    afl::data::StringList_t obj;
    obj.push_back("one");
    obj.push_back(String_t("two"));
    TS_ASSERT_EQUALS(obj.size(), 2U);
    TS_ASSERT_EQUALS(obj[0], "one");
}
