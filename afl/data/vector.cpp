/**
  *  \file afl/data/vector.cpp
  *  \brief Class afl::data::Vector
  */

#include "afl/data/vector.hpp"

inline
afl::data::Vector::Vector()
    : m_values()
{ }

afl::data::Vector::~Vector()
{ }

afl::base::Ptr<afl::data::Vector>
afl::data::Vector::create()
{
    return new Vector();
}

afl::base::Ptr<afl::data::Vector>
afl::data::Vector::create(Segment& values)
{
    afl::base::Ptr<afl::data::Vector> vec = new Vector();
    vec->m_values.swap(values);
    return vec;
}

void
afl::data::Vector::set(Index_t index, Value* value)
{
    m_values.set(index, value);
}

void
afl::data::Vector::setNew(Index_t index, Value* value)
{
    m_values.setNew(index, value);
}

afl::data::Value*
afl::data::Vector::get(Index_t index) const
{
    return m_values[index];
}

const afl::data::Segment&
afl::data::Vector::getValues() const
{
    return m_values;
}

afl::data::Segment&
afl::data::Vector::getValues()
{
    return m_values;
}

afl::data::Vector::Index_t
afl::data::Vector::size() const
{
    return m_values.size();
}

void
afl::data::Vector::pushBack(Value* value)
{
    m_values.pushBack(value);
}

void
afl::data::Vector::pushBackNew(Value* value)
{
    m_values.pushBackNew(value);
}

