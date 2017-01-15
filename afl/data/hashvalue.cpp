/**
  *  \file afl/data/hashvalue.cpp
  *  \brief Class afl::data::HashValue
  */

#include "afl/data/hashvalue.hpp"
#include "afl/data/visitor.hpp"

afl::data::HashValue::HashValue(afl::base::Ref<Hash> pHash)
    : Value(),
      m_pHash(pHash)
{ }

afl::data::HashValue::~HashValue()
{ }

void
afl::data::HashValue::visit(Visitor& visitor) const
{
    visitor.visitHash(*m_pHash);
}

afl::data::HashValue*
afl::data::HashValue::clone() const
{
    return new HashValue(m_pHash);
}
