/**
  *  \file afl/data/booleanvalue.cpp
  *  \brief Class afl::data::BooleanValue
  */

#include "afl/data/booleanvalue.hpp"
#include "afl/data/visitor.hpp"

afl::data::BooleanValue::BooleanValue(bool value)
    : ScalarValue(value)
{ }

afl::data::BooleanValue::~BooleanValue()
{ }

void
afl::data::BooleanValue::visit(Visitor& visitor) const
{
    visitor.visitBoolean(getValue() != 0);
}

afl::data::BooleanValue*
afl::data::BooleanValue::clone() const
{
    return new BooleanValue(getValue());
}
