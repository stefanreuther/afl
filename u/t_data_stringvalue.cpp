/**
  *  \file u/t_data_stringvalue.cpp
  *  \brief Test for afl::data::StringValue
  */

#include "afl/data/stringvalue.hpp"

#include "u/t_data.hpp"

/** Simple test. */
void
TestDataStringValue::testIt()
{
    const afl::data::StringValue sv("");
    TS_ASSERT_EQUALS(sv.getValue(), "");

    const afl::data::StringValue sv2("xyz");
    TS_ASSERT_EQUALS(sv2.getValue(), "xyz");
}
