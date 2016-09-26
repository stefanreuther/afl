/**
  *  \file afl/data/stringvalue.cpp
  *  \brief Class afl::data::StringValue
  */

#include "afl/data/stringvalue.hpp"
#include "afl/data/visitor.hpp"

afl::data::StringValue::StringValue(const String_t& value)
    : Value(),
      m_value(value)
{ }

afl::data::StringValue::~StringValue()
{ }

void
afl::data::StringValue::visit(Visitor& visitor) const
{
    visitor.visitString(m_value);
}

afl::data::StringValue*
afl::data::StringValue::clone() const
{
    return new StringValue(m_value);
}
