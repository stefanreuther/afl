/**
  *  \file afl/net/redis/integersetkey.cpp
  *  \brief Class afl::net::redis::IntegerSetKey
  */

#include <memory>
#include "afl/net/redis/integersetkey.hpp"
#include "afl/data/segment.hpp"
#include "afl/data/access.hpp"
#include "afl/net/redis/integersetoperation.hpp"

using afl::data::Segment;

afl::net::redis::IntegerSetKey::IntegerSetKey(CommandHandler& ch, const String_t& name)
    : SetKey(ch, name)
{ }

// Add a value to the set (SADD).
bool
afl::net::redis::IntegerSetKey::add(int32_t value)
{
    return getHandler().callInt(Segment().pushBackString("SADD").pushBackString(getName()).pushBackInteger(value));
}

// Remove a value from the set (SREM).
bool
afl::net::redis::IntegerSetKey::remove(int32_t value)
{
    return getHandler().callInt(Segment().pushBackString("SREM").pushBackString(getName()).pushBackInteger(value));
}

// Extract a random value (SPOP).
int32_t
afl::net::redis::IntegerSetKey::extractRandom()
{
    return getHandler().callInt(Segment().pushBackString("SPOP").pushBackString(getName()));
}

// Get a random value (SRANDMEMBER).
int32_t
afl::net::redis::IntegerSetKey::getRandom() const
{
    return getHandler().callInt(Segment().pushBackString("SRANDMEMBER").pushBackString(getName()));
}

// Move a value into another set (SMOVE).
bool
afl::net::redis::IntegerSetKey::moveTo(int32_t value, const IntegerSetKey& other)
{
    return getHandler().callInt(Segment().pushBackString("SMOVE").pushBackString(getName()).pushBackString(other.getName()).pushBackInteger(value));
}

// Check whether set contains an element (SISMEMBER).
bool
afl::net::redis::IntegerSetKey::contains(int32_t value) const
{
    return getHandler().callInt(Segment().pushBackString("SISMEMBER").pushBackString(getName()).pushBackInteger(value));
}

// Copy whole set (SINTERSTORE).
void
afl::net::redis::IntegerSetKey::copyTo(const IntegerSetKey& other)
{
    getHandler().callVoid(Segment().pushBackString("SINTERSTORE").pushBackString(other.getName()).pushBackString(getName()));
}

// Get all elements (SMEMBERS).
void
afl::net::redis::IntegerSetKey::getAll(afl::data::IntegerList_t& list) const
{
    std::auto_ptr<afl::data::Value> val(getHandler().call(Segment().pushBackString("SMEMBERS").pushBackString(getName())));
    afl::data::Access(val).toIntegerList(list);
}

// Compute intersection of sets (SINTER, SINTERSTORE).
afl::net::redis::IntegerSetOperation
afl::net::redis::IntegerSetKey::intersect(const IntegerSetKey& other) const
{
    return IntegerSetOperation("SINTER", *this, other);
}

// Compute union of sets (SUNION, SUNIONSTORE).
afl::net::redis::IntegerSetOperation
afl::net::redis::IntegerSetKey::merge(const IntegerSetKey& other) const
{
    return IntegerSetOperation("SUNION", *this, other);
}

// Compute difference of sets (SDIFF, SDIFFSTORE).
afl::net::redis::IntegerSetOperation
afl::net::redis::IntegerSetKey::remove(const IntegerSetKey& other) const
{
    return IntegerSetOperation("SDIFF", *this, other);
}
