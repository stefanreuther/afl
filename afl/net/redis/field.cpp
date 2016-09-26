/**
  *  \file afl/net/redis/field.cpp
  *  \brief Class afl::net::redis::Field
  */

#include "afl/net/redis/field.hpp"
#include "afl/net/commandhandler.hpp"
#include "afl/data/segment.hpp"

using afl::data::Segment;

// Constructor.
afl::net::redis::Field::Field(HashKey& hash, const String_t& name)
    : m_hash(hash),
      m_name(name)
{ }

// Destructor.
afl::net::redis::Field::~Field()
{ }

// Check for existence (HEXISTS).
bool
afl::net::redis::Field::exists() const
{
    return getHash().getHandler().callInt(Segment()
                                          .pushBack("HEXISTS")
                                          .pushBack(getHash().getName())
                                          .pushBack(getName()));
}

// Remove this field from the hash (HDEL).
bool
afl::net::redis::Field::remove()
{
    return getHash().getHandler().callInt(Segment()
                                          .pushBack("HDEL")
                                          .pushBack(getHash().getName())
                                          .pushBack(getName()));
}

// Get name of this field.
const String_t&
afl::net::redis::Field::getName() const
{
    return m_name;
}

const afl::net::redis::HashKey&
afl::net::redis::Field::getHash() const
{
    return m_hash;
}
