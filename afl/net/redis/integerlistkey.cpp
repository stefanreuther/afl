/**
  *  \file afl/net/redis/integerlistkey.cpp
  *  \brief Class afl::net::redis::IntegerListKey
  */

#include <memory>
#include "afl/net/redis/integerlistkey.hpp"
#include "afl/data/segment.hpp"
#include "afl/data/access.hpp"

using afl::data::Segment;

// Constructor.
afl::net::redis::IntegerListKey::IntegerListKey(CommandHandler& ch, String_t name)
    : ListKey(ch, name)
{ }

// Get element by index (LINDEX).
int32_t
afl::net::redis::IntegerListKey::operator[](int32_t index) const
{
    return getHandler().callInt(Segment().pushBackString("LINDEX").pushBackString(getName()).pushBackInteger(index));
}

// Extract first element (LPOP).
int32_t
afl::net::redis::IntegerListKey::popFront()
{
    return getHandler().callInt(Segment().pushBackString("LPOP").pushBackString(getName()));
}

// Insert element at beginning (LPUSH).
int32_t
afl::net::redis::IntegerListKey::pushFront(int32_t value)
{
    return getHandler().callInt(Segment().pushBackString("LPUSH").pushBackString(getName()).pushBackInteger(value));
}

// Extract last element (RPOP).
int32_t
afl::net::redis::IntegerListKey::popBack()
{
    return getHandler().callInt(Segment().pushBackString("RPOP").pushBackString(getName()));
}

// Insert element at end (RPUSH).
int32_t
afl::net::redis::IntegerListKey::pushBack(int32_t value)
{
    return getHandler().callInt(Segment().pushBackString("RPUSH").pushBackString(getName()).pushBackInteger(value));
}

// Set element at index (LSET).
void
afl::net::redis::IntegerListKey::set(int32_t index, int32_t value)
{
    getHandler().callVoid(Segment().pushBackString("LSET").pushBackString(getName()).pushBackInteger(index).pushBackInteger(value));
}

// Remove elements by value (LREM).
int32_t
afl::net::redis::IntegerListKey::removeValue(int32_t value, int32_t count)
{
    return getHandler().callInt(Segment().pushBackString("LREM").pushBackString(getName()).pushBackInteger(count).pushBackInteger(value));
}

// Transfer element to another list (RPOPLPUSH).
int32_t
afl::net::redis::IntegerListKey::transferElement(IntegerListKey& other)
{
    return getHandler().callInt(Segment().pushBackString("RPOPLPUSH").pushBackString(getName()).pushBackString(other.getName()));
}

// Get subrange of list (LRANGE).
void
afl::net::redis::IntegerListKey::getRange(int32_t start, int32_t end, afl::data::IntegerList_t& list) const
{
    std::auto_ptr<afl::data::Value> val(getHandler().call(Segment().pushBackString("LRANGE").pushBackString(getName()).pushBackInteger(start).pushBackInteger(end)));
    afl::data::Access(val).toIntegerList(list);
}

// Get all elements of the list (LRANGE).
void
afl::net::redis::IntegerListKey::getAll(afl::data::IntegerList_t& list) const
{
    getRange(0, -1, list);
}
