/**
  *  \file afl/data/vector.cpp
  *  \brief Class afl::data::Vector
  */

#include "afl/data/vector.hpp"

inline
afl::data::Vector::Vector()
    : RefCounted(),
      Segment()
{ }

afl::data::Vector::~Vector()
{ }

afl::data::Vector::Ref_t
afl::data::Vector::create()
{
    return *new Vector();
}

afl::data::Vector::Ref_t
afl::data::Vector::create(Segment& values)
{
    afl::base::Ref<afl::data::Vector> vec = *new Vector();
    vec->swap(values);
    return vec;
}
