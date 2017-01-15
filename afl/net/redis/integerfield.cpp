/**
  *  \file afl/net/redis/integerfield.cpp
  *  \brief Class afl::net::redis::IntegerField
  */

#include "afl/net/redis/integerfield.hpp"
#include "afl/data/segment.hpp"
#include "afl/data/integervalue.hpp"

using afl::data::Segment;

// Constructor.
afl::net::redis::IntegerField::IntegerField(HashKey& hash, const String_t& name)
    : Field(hash, name)
{ }

// Increment by one (HINCRBY).
int32_t
afl::net::redis::IntegerField::operator++()
{
    return operator+=(1);
}

// Increment (HINCRBY).
int32_t
afl::net::redis::IntegerField::operator+=(int32_t incr)
{
    return getHash().getHandler().callInt(Segment()
                                          .pushBackString("HINCRBY")
                                          .pushBackString(getHash().getName())
                                          .pushBackString(getName())
                                          .pushBackInteger(incr));
}

// Increment by one (HINCRBY).
int32_t
afl::net::redis::IntegerField::operator--()
{
    return operator+=(-1);
}

// Decrement (HINCRBY).
int32_t
afl::net::redis::IntegerField::operator-=(int32_t incr)
{
    return operator+=(-incr);
}

// Set value (HSET).
bool
afl::net::redis::IntegerField::set(int32_t value)
{
    return getHash().getHandler().callInt(Segment()
                                          .pushBackString("HSET")
                                          .pushBackString(getHash().getName())
                                          .pushBackString(getName())
                                          .pushBackInteger(value));
}

// Get current value (HGET).
int32_t
afl::net::redis::IntegerField::get() const
{
    return getHash().getHandler().callInt(Segment()
                                          .pushBackString("HGET")
                                          .pushBackString(getHash().getName())
                                          .pushBackString(getName()));
}
