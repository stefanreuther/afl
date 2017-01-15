/**
  *  \file afl/net/redis/stringfield.cpp
  *  \brief Class afl::net::redis::StringField
  */

#include "afl/net/redis/stringfield.hpp"
#include "afl/data/segment.hpp"

using afl::data::Segment;

// Constructor.
afl::net::redis::StringField::StringField(HashKey& hash, const String_t& name)
    : Field(hash, name)
{ }

// Set value (HSET).
bool
afl::net::redis::StringField::set(String_t value)
{
    return getHash().getHandler().callInt(Segment()
                                          .pushBackString("HSET")
                                          .pushBackString(getHash().getName())
                                          .pushBackString(getName())
                                          .pushBackString(value));
}

// Get current value (HGET).
String_t
afl::net::redis::StringField::get() const
{
    return getHash().getHandler().callString(Segment()
                                             .pushBackString("HGET")
                                             .pushBackString(getHash().getName())
                                             .pushBackString(getName()));
}
