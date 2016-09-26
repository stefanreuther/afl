/**
  *  \file afl/net/redis/stringsetkey.cpp
  *  \brief Class afl::net::redis::StringSetKey
  */

#include <memory>
#include "afl/net/redis/stringsetkey.hpp"
#include "afl/data/segment.hpp"
#include "afl/data/access.hpp"
#include "afl/net/redis/stringsetoperation.hpp"

using afl::data::Segment;

afl::net::redis::StringSetKey::StringSetKey(CommandHandler& ch, const String_t& name)
    : SetKey(ch, name)
{ }

// Add a value to the set (SADD).
bool
afl::net::redis::StringSetKey::add(String_t value)
{
    return getHandler().callInt(Segment().pushBack("SADD").pushBack(getName()).pushBack(value));
}

// Remove a value from the set (SREM).
bool
afl::net::redis::StringSetKey::remove(String_t value)
{
    return getHandler().callInt(Segment().pushBack("SREM").pushBack(getName()).pushBack(value));
}

// Extract a random value (SPOP).
String_t
afl::net::redis::StringSetKey::extractRandom()
{
    return getHandler().callString(Segment().pushBack("SPOP").pushBack(getName()));
}

// Get a random value (SRANDMEMBER).
String_t
afl::net::redis::StringSetKey::getRandom() const
{
    return getHandler().callString(Segment().pushBack("SRANDMEMBER").pushBack(getName()));
}

// Move a value into another set (SMOVE).
bool
afl::net::redis::StringSetKey::moveTo(String_t value, const StringSetKey& other)
{
    return getHandler().callInt(Segment().pushBack("SMOVE").pushBack(getName()).pushBack(other.getName()).pushBack(value));
}

// Check whether set contains an element (SISMEMBER).
bool
afl::net::redis::StringSetKey::contains(String_t value) const
{
    return getHandler().callInt(Segment().pushBack("SISMEMBER").pushBack(getName()).pushBack(value));
}

// Copy whole set (SINTERSTORE).
void
afl::net::redis::StringSetKey::copyTo(const StringSetKey& other)
{
    getHandler().callVoid(Segment().pushBack("SINTERSTORE").pushBack(other.getName()).pushBack(getName()));
}

// Get all elements (SMEMBERS).
void
afl::net::redis::StringSetKey::getAll(afl::data::StringList_t& list) const
{
    std::auto_ptr<afl::data::Value> val(getHandler().call(Segment().pushBack("SMEMBERS").pushBack(getName())));
    afl::data::Access(val).toStringList(list);
}

// Compute intersection of sets (SINTER, SINTERSTORE).
afl::net::redis::StringSetOperation
afl::net::redis::StringSetKey::intersect(const StringSetKey& other) const
{
    return StringSetOperation("SINTER", *this, other);
}

// Compute union of sets (SUNION, SUNIONSTORE).
afl::net::redis::StringSetOperation
afl::net::redis::StringSetKey::merge(const StringSetKey& other) const
{
    return StringSetOperation("SUNION", *this, other);
}

// Compute difference of sets (SDIFF, SDIFFSTORE).
afl::net::redis::StringSetOperation
afl::net::redis::StringSetKey::remove(const StringSetKey& other) const
{
    return StringSetOperation("SDIFF", *this, other);
}
