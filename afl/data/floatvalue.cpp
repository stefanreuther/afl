/**
  *  \file afl/data/floatvalue.cpp
  *  \brief Class afl::data::FloatValue
  */

#include "afl/data/floatvalue.hpp"
#include "afl/data/visitor.hpp"

afl::data::FloatValue::FloatValue(double value)
    : Value(),
      m_value(value)
{ }

afl::data::FloatValue::~FloatValue()
{ }

void
afl::data::FloatValue::visit(Visitor& visitor) const
{
    visitor.visitFloat(m_value);
}

afl::data::FloatValue*
afl::data::FloatValue::clone() const
{
    return new FloatValue(m_value);
}

void
afl::data::FloatValue::add(double delta)
{
    m_value += delta;
}
