/**
  *  \file afl/data/vectorvalue.cpp
  *  \brief Class afl::data::VectorValue
  */

#include <cassert>
#include "afl/data/vectorvalue.hpp"
#include "afl/data/visitor.hpp"

afl::data::VectorValue::VectorValue(afl::base::Ptr<Vector> pVector)
    : Value(),
      m_pVector(pVector)
{
    assert(m_pVector.get() != 0);
}

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
