/**
  *  \file u/t_data_floatvalue.cpp
  *  \brief Test for afl::data::FloatValue
  */

#include <memory>
#include "afl/data/floatvalue.hpp"

#include "u/t_data.hpp"

/** Simple test. */
void
TestDataFloatValue::testIt()
{
    const afl::data::FloatValue fv(99);
    TS_ASSERT_EQUALS(fv.getValue(), 99);

    const afl::data::FloatValue fv2(-10);
    TS_ASSERT_EQUALS(fv2.getValue(), -10);

    const afl::data::FloatValue fv3(1.75);
    TS_ASSERT_EQUALS(fv3.getValue(), 1.75);

    const afl::data::FloatValue fv4(1e20);
    TS_ASSERT_EQUALS(fv4.getValue(), 1e20);

    std::auto_ptr<afl::data::FloatValue> clone(fv.clone());
    TS_ASSERT(clone.get() != 0);
    TS_ASSERT_EQUALS(clone->getValue(), 99);
    clone->add(2);
    TS_ASSERT_EQUALS(clone->getValue(), 101);
    clone->add(0.5);
    TS_ASSERT_EQUALS(clone->getValue(), 101.5);
}
