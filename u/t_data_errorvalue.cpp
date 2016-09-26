/**
  *  \file u/t_data_errorvalue.cpp
  *  \brief Test for afl::data::ErrorValue
  */

#include "afl/data/errorvalue.hpp"

#include "u/t_data.hpp"

/** Simple test. */
void
TestDataErrorValue::testIt()
{
    const afl::data::ErrorValue ev("", "");
    TS_ASSERT_EQUALS(ev.getText(), "");
    TS_ASSERT_EQUALS(ev.getSource(), "");

    const afl::data::ErrorValue ev2("src", "xyz");
    TS_ASSERT_EQUALS(ev2.getText(), "xyz");
    TS_ASSERT_EQUALS(ev2.getSource(), "src");
}
