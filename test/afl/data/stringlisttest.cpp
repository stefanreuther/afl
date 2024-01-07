/**
  *  \file test/afl/data/stringlisttest.cpp
  *  \brief Test for afl::data::StringList
  */

#include "afl/data/stringlist.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.data.StringList", a)
{
    afl::data::StringList_t obj;
    obj.push_back("one");
    obj.push_back(String_t("two"));
    a.checkEqual("size", obj.size(), 2U);
    a.checkEqual("value", obj[0], "one");
}
