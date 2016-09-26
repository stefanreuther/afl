/**
  *  \file afl/data/integervalue.cpp
  *  \brief Class afl::data::IntegerValue
  */

#include "afl/data/integervalue.hpp"
#include "afl/data/visitor.hpp"

afl::data::IntegerValue::IntegerValue(int32_t value)
    : ScalarValue(value)
{ }

afl::data::IntegerValue::~IntegerValue()
{ }

void
afl::data::IntegerValue::visit(Visitor& visitor) const
{
    visitor.visitInteger(getValue());
}

afl::data::IntegerValue*
afl::data::IntegerValue::clone() const
{
    return new IntegerValue(getValue());
}

void
afl::data::IntegerValue::add(int32_t delta)
{
    setValue(delta + getValue());
}
