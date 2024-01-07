/**
  *  \file test/afl/data/integervaluetest.cpp
  *  \brief Test for afl::data::IntegerValue
  */

#include "afl/data/integervalue.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.data.IntegerValue:create:pos", a) {
    const afl::data::IntegerValue iv(99);
    a.checkEqual("value", iv.getValue(), 99);
}

AFL_TEST("afl.data.IntegerValue:create:neg", a) {
    const afl::data::IntegerValue iv2(-77);
    a.checkEqual("value", iv2.getValue(), -77);
    a.check("sign", iv2.getValue() < 0);
}

AFL_TEST("afl.data.IntegerValue:scalar", a) {
    const afl::data::IntegerValue iv(99);
    const afl::data::ScalarValue* sv = &iv;
    a.checkEqual("value", sv->getValue(), 99);
}

AFL_TEST("afl.data.IntegerValue:clone", a) {
    const afl::data::IntegerValue iv(99);
    std::auto_ptr<afl::data::IntegerValue> clone(iv.clone());
    a.check("clone", clone.get() != 0);
    a.checkEqual("value", clone->getValue(), 99);
    clone->add(2);
    a.checkEqual("value after add", clone->getValue(), 101);
}
