/**
  *  \file u/t_data_hashvalue.cpp
  *  \brief Test for afl::data::HashValue
  */

#include "afl/data/hashvalue.hpp"

#include <memory>
#include "u/t_data.hpp"

/** Simple test. */
void
TestDataHashValue::testIt()
{
    // Create one value
    std::auto_ptr<afl::data::HashValue> hv(new afl::data::HashValue(afl::data::Hash::create()));

    // Create another
    std::auto_ptr<afl::data::HashValue> hv2(hv->clone());
    TS_ASSERT(hv2.get() != 0);

    // Must reference the same hash
    TS_ASSERT(hv->getValue() == hv2->getValue());
}
