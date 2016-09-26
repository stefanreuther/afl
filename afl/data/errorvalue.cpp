/**
  *  \file afl/data/errorvalue.cpp
  *  \brief Class afl::data::ErrorValue
  */

#include "afl/data/errorvalue.hpp"
#include "afl/data/visitor.hpp"

afl::data::ErrorValue::ErrorValue(const String_t& source, const String_t& str)
    : Value(),
      m_source(source),
      m_text(str)
{ }

afl::data::ErrorValue::~ErrorValue()
{ }

void
afl::data::ErrorValue::visit(Visitor& visitor) const
{
    visitor.visitError(m_source, m_text);
}

afl::data::ErrorValue*
afl::data::ErrorValue::clone() const
{
    return new ErrorValue(m_source, m_text);
}

const String_t&
afl::data::ErrorValue::getText() const
{
    return m_text;
}

const String_t&
afl::data::ErrorValue::getSource() const
{
    return m_source;
}
