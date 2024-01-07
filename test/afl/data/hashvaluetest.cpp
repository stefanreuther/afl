/**
  *  \file test/afl/data/hashvaluetest.cpp
  *  \brief Test for afl::data::HashValue
  */

#include "afl/data/hashvalue.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.data.HashValue", a)
{
    // Create one value
    std::auto_ptr<afl::data::HashValue> hv(new afl::data::HashValue(afl::data::Hash::create()));

    // Create another
    std::auto_ptr<afl::data::HashValue> hv2(hv->clone());
    a.check("clone", hv2.get() != 0);

    // Must reference the same hash
    a.check("getValue", &hv->getValue().get() == &hv2->getValue().get());
}
