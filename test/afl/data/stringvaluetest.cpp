/**
  *  \file test/afl/data/stringvaluetest.cpp
  *  \brief Test for afl::data::StringValue
  */

#include "afl/data/stringvalue.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.data.StringValue", a)
{
    const afl::data::StringValue sv("");
    a.checkEqual("empty value", sv.getValue(), "");

    const afl::data::StringValue sv2("xyz");
    a.checkEqual("nonempty value", sv2.getValue(), "xyz");
}
