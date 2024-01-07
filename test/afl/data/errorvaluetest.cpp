/**
  *  \file test/afl/data/errorvaluetest.cpp
  *  \brief Test for afl::data::ErrorValue
  */

#include "afl/data/errorvalue.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.data.ErrorValue", a)
{
    const afl::data::ErrorValue ev("", "");
    a.checkEqual("text 1", ev.getText(), "");
    a.checkEqual("source 1", ev.getSource(), "");

    const afl::data::ErrorValue ev2("src", "xyz");
    a.checkEqual("text 2", ev2.getText(), "xyz");
    a.checkEqual("source 2", ev2.getSource(), "src");
}
