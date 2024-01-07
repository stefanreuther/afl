/**
  *  \file test/afl/data/vectorvaluetest.cpp
  *  \brief Test for afl::data::VectorValue
  */

#include "afl/data/vectorvalue.hpp"

#include <memory>
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.data.VectorValue", a)
{
    // Create one value
    std::auto_ptr<afl::data::VectorValue> vv(new afl::data::VectorValue(afl::data::Vector::create()));

    // Create another
    std::auto_ptr<afl::data::VectorValue> vv2(vv->clone());
    a.check("clone", vv2.get() != 0);

    // Must reference the same vector
    a.check("value", &vv->getValue().get() == &vv2->getValue().get());
}
