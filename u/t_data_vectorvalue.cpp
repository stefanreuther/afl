/**
  *  \file u/t_data_vectorvalue.cpp
  *  \brief Test for afl::data::VectorValue
  */

#include "afl/data/vectorvalue.hpp"

#include <memory>
#include "u/t_data.hpp"

/** Simple test. */
void
TestDataVectorValue::testIt()
{
    // Create one value
    std::auto_ptr<afl::data::VectorValue> vv(new afl::data::VectorValue(afl::data::Vector::create()));

    // Create another
    std::auto_ptr<afl::data::VectorValue> vv2(vv->clone());
    TS_ASSERT(vv2.get() != 0);

    // Must reference the same vector
    TS_ASSERT(vv->getValue() == vv2->getValue());
}
