/**
  *  \file test/afl/data/namemaptest.cpp
  *  \brief Test for afl::data::NameMap
  */

#include "afl/data/namemap.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.data.NameMap", a)
{
    afl::data::NameMap names;
    a.checkEqual("01. getNumNames", names.getNumNames(), 0U);
    a.checkEqual("02. getIndexByName", names.getIndexByName("a"), names.nil);

    a.checkEqual("11. add", names.add("a"), 0U);
    a.checkEqual("12. add", names.add("b"), 1U);
    a.checkEqual("13. add", names.add("c"), 2U);

    a.checkEqual("11. getIndexByName", names.getIndexByName("a"), 0U);
    a.checkEqual("12. getIndexByName", names.getIndexByName("b"), 1U);
    a.checkEqual("13. getIndexByName", names.getIndexByName("c"), 2U);

    a.checkEqual("21. getNameByIndex", names.getNameByIndex(0), "a");
    a.checkEqual("22. getNameByIndex", names.getNameByIndex(1), "b");
    a.checkEqual("23. getNameByIndex", names.getNameByIndex(2), "c");

    a.checkEqual("31. addMaybe", names.addMaybe("a"), 0U);
    a.checkEqual("32. addMaybe", names.addMaybe("b"), 1U);
    a.checkEqual("33. addMaybe", names.addMaybe("c"), 2U);

    a.checkEqual("41. add", names.add("a"), 3U);
    a.checkEqual("42. getIndexByName", names.getIndexByName("a"), 3U);
    a.checkEqual("43. addMaybe", names.addMaybe("a"), 3U);
    a.checkEqual("44. getNameByIndex", names.getNameByIndex(3), "a");
    a.checkEqual("45. getNameByIndex", names.getNameByIndex(0), "a");
}
