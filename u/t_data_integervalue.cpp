/**
  *  \file u/t_data_integervalue.cpp
  *  \brief Test for afl::data::IntegerValue
  */

#include "afl/data/integervalue.hpp"

#include "u/t_data.hpp"

/** Simple test. */
void
TestDataIntegerValue::testIt()
{
    const afl::data::IntegerValue iv(99);
    TS_ASSERT_EQUALS(iv.getValue(), 99);

    const afl::data::IntegerValue iv2(-77);
    TS_ASSERT_EQUALS(iv2.getValue(), -77);
    TS_ASSERT(iv2.getValue() < 0);

    const afl::data::ScalarValue* sv = &iv;
    TS_ASSERT_EQUALS(sv->getValue(), 99);
}
