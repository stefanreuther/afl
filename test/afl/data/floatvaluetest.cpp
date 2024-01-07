/**
  *  \file test/afl/data/floatvaluetest.cpp
  *  \brief Test for afl::data::FloatValue
  */

#include "afl/data/floatvalue.hpp"

#include <memory>
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.data.FloatValue:create:pos", a) {
    const afl::data::FloatValue fv(99);
    a.checkEqual("", fv.getValue(), 99);
}

AFL_TEST("afl.data.FloatValue:create:neg", a) {
    const afl::data::FloatValue fv2(-10);
    a.checkEqual("", fv2.getValue(), -10);
}

AFL_TEST("afl.data.FloatValue:create:fract", a) {
    const afl::data::FloatValue fv3(1.75);
    a.checkEqual("", fv3.getValue(), 1.75);
}

AFL_TEST("afl.data.FloatValue:create:large", a) {
    const afl::data::FloatValue fv4(1e20);
    a.checkEqual("", fv4.getValue(), 1e20);
}

AFL_TEST("afl.data.FloatValue:clone", a) {
    const afl::data::FloatValue fv(99);
    std::auto_ptr<afl::data::FloatValue> clone(fv.clone());
    a.check("result", clone.get() != 0);
    a.checkEqual("value", clone->getValue(), 99);
    clone->add(2);
    a.checkEqual("value after add int", clone->getValue(), 101);
    clone->add(0.5);
    a.checkEqual("value after add float", clone->getValue(), 101.5);
}
