/**
  *  \file afl/data/vectorvalue.cpp
  *  \brief Class afl::data::VectorValue
  */

#include "afl/data/vectorvalue.hpp"
#include "afl/data/visitor.hpp"

afl::data::VectorValue::VectorValue(afl::base::Ref<Vector> pVector)
    : Value(),
      m_pVector(pVector)
{ }

afl::data::VectorValue::~VectorValue()
{ }

void
afl::data::VectorValue::visit(Visitor& visitor) const
{
    visitor.visitVector(*m_pVector);
}

afl::data::VectorValue*
afl::data::VectorValue::clone() const
{
    return new VectorValue(m_pVector);
}
