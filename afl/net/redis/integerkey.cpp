/**
  *  \file afl/net/redis/integerkey.cpp
  *  \brief Class afl::net::redis::IntegerKey
  */

#include "afl/net/redis/integerkey.hpp"
#include "afl/data/segment.hpp"

using afl::data::Segment;

// Constructor.
afl::net::redis::IntegerKey::IntegerKey(CommandHandler& ch, String_t name)
    : Key(ch, name)
{ }

// Decrement by one (DECR).
int32_t
afl::net::redis::IntegerKey::operator--()
{
    return getHandler().callInt(Segment().pushBackString("DECR").pushBackString(getName()));
}

// Decrement (DECRBY).
int32_t
afl::net::redis::IntegerKey::operator-=(int32_t val)
{
    return getHandler().callInt(Segment().pushBackString("DECRBY").pushBackString(getName()).pushBackInteger(val));
}

// Get current value (GET).
int32_t
afl::net::redis::IntegerKey::get()
{
    return getHandler().callInt(Segment().pushBackString("GET").pushBackString(getName()));
}

// Replace with a new value (GETSET).
int32_t
afl::net::redis::IntegerKey::replaceBy(int32_t newValue)
{
    return getHandler().callInt(Segment().pushBackString("GETSET").pushBackString(getName()).pushBackInteger(newValue));
}

// Increment by one (INCR).
int32_t
afl::net::redis::IntegerKey::operator++()
{
    return getHandler().callInt(Segment().pushBackString("INCR").pushBackString(getName()));
}

// Increment (INCRBY).
int32_t
afl::net::redis::IntegerKey::operator+=(int32_t val)
{
    return getHandler().callInt(Segment().pushBackString("INCRBY").pushBackString(getName()).pushBackInteger(val));
}

// Set value (SET).
void
afl::net::redis::IntegerKey::set(int32_t newValue)
{
    getHandler().callVoid(Segment().pushBackString("SET").pushBackString(getName()).pushBackInteger(newValue));
}

// Set value for new key (SETNX).
bool
afl::net::redis::IntegerKey::setUnique(int32_t newValue)
{
    return getHandler().callInt(Segment().pushBackString("SETNX").pushBackString(getName()).pushBackInteger(newValue));
}
