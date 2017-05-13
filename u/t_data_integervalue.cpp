/**
  *  \file u/t_data_integervalue.cpp
  *  \brief Test for afl::data::IntegerValue
  */

#include <memory>
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

    std::auto_ptr<afl::data::IntegerValue> clone(iv.clone());
    TS_ASSERT(clone.get() != 0);
    TS_ASSERT_EQUALS(clone->getValue(), 99);
    clone->add(2);
    TS_ASSERT_EQUALS(clone->getValue(), 101);
}
