/**
  *  \file u/t_data_booleanvalue.cpp
  *  \brief Test for afl::data::BooleanValue
  */

#include <memory>
#include "afl/data/booleanvalue.hpp"

#include "u/t_data.hpp"

/** Simple test. */
void
TestDataBooleanValue::testIt()
{
    // Construct with actual bools
    const afl::data::BooleanValue bv(true);
    TS_ASSERT_EQUALS(bv.getValue(), 1);

    const afl::data::BooleanValue bv2(false);
    TS_ASSERT_EQUALS(bv2.getValue(), 0);

    // Conversion to bool during construction
    const afl::data::BooleanValue bv3(99);
    TS_ASSERT_EQUALS(bv3.getValue(), 1);

    // Conversion to ScalarValue
    const afl::data::ScalarValue* sv = &bv3;
    TS_ASSERT_EQUALS(sv->getValue(), 1);

    // Clone
    std::auto_ptr<afl::data::BooleanValue> clone(bv.clone());
    TS_ASSERT(clone.get() != 0);
    TS_ASSERT_EQUALS(clone->getValue(), 1);

    clone.reset(bv2.clone());
    TS_ASSERT(clone.get() != 0);
    TS_ASSERT_EQUALS(clone->getValue(), 0);
}
