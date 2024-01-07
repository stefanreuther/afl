/**
  *  \file test/afl/data/booleanvaluetest.cpp
  *  \brief Test for afl::data::BooleanValue
  */

#include "afl/data/booleanvalue.hpp"
#include "afl/test/testrunner.hpp"

// Construct with actual bools
AFL_TEST("afl.data.BooleanValue:from-true", a) {
    const afl::data::BooleanValue bv(true);
    a.checkEqual("", bv.getValue(), 1);
}

AFL_TEST("afl.data.BooleanValue:from-false", a) {
    const afl::data::BooleanValue bv2(false);
    a.checkEqual("", bv2.getValue(), 0);
}

// Conversion to bool during construction
AFL_TEST("afl.data.BooleanValue:from-int", a) {
    const afl::data::BooleanValue bv3(99);
    a.checkEqual("", bv3.getValue(), 1);
}

// Conversion to ScalarValue
AFL_TEST("afl.data.BooleanValue:is-scalar", a) {
    const afl::data::BooleanValue bv3(99);
    const afl::data::ScalarValue* sv = &bv3;
    a.checkEqual("", sv->getValue(), 1);
}

// Clone
AFL_TEST("afl.data.BooleanValue:clone", a) {
    const afl::data::BooleanValue bv(true);
    const afl::data::BooleanValue bv2(false);

    std::auto_ptr<afl::data::BooleanValue> clone(bv.clone());
    a.checkNonNull("clone", clone.get());
    a.checkEqual("cloned value", clone->getValue(), 1);

    clone.reset(bv2.clone());
    a.checkNonNull("clone 2", clone.get());
    a.checkEqual("cloned value 2", clone->getValue(), 0);
}
