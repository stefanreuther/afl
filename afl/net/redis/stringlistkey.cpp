/**
  *  \file afl/net/redis/stringlistkey.cpp
  *  \brief Class afl::net::redis::StringListKey
  */

#include <memory>
#include "afl/net/redis/stringlistkey.hpp"
#include "afl/data/segment.hpp"
#include "afl/data/access.hpp"

using afl::data::Segment;

// Constructor.
afl::net::redis::StringListKey::StringListKey(CommandHandler& ch, String_t name)
    : ListKey(ch, name)
{ }

// Get element by index (LINDEX).
String_t
afl::net::redis::StringListKey::operator[](int32_t index) const
{
    return getHandler().callString(Segment().pushBackString("LINDEX").pushBackString(getName()).pushBackInteger(index));
}

// Extract first element (LPOP).
String_t
afl::net::redis::StringListKey::popFront()
{
    return getHandler().callString(Segment().pushBackString("LPOP").pushBackString(getName()));
}

// Insert element at beginning (LPUSH).
int32_t
afl::net::redis::StringListKey::pushFront(String_t value)
{
    return getHandler().callInt(Segment().pushBackString("LPUSH").pushBackString(getName()).pushBackString(value));
}

// Extract last element (RPOP).
String_t
afl::net::redis::StringListKey::popBack()
{
    return getHandler().callString(Segment().pushBackString("RPOP").pushBackString(getName()));
}

// Insert element at end (RPUSH).
int32_t
afl::net::redis::StringListKey::pushBack(String_t value)
{
    return getHandler().callInt(Segment().pushBackString("RPUSH").pushBackString(getName()).pushBackString(value));
}

// Set element at index (LSET).
void
afl::net::redis::StringListKey::set(int32_t index, String_t value)
{
    getHandler().callVoid(Segment().pushBackString("LSET").pushBackString(getName()).pushBackInteger(index).pushBackString(value));
}

// Remove elements by value (LREM).
int32_t
afl::net::redis::StringListKey::removeValue(String_t value, int32_t count)
{
    return getHandler().callInt(Segment().pushBackString("LREM").pushBackString(getName()).pushBackInteger(count).pushBackString(value));
}

// Transfer element to another list (RPOPLPUSH).
String_t
afl::net::redis::StringListKey::transferElement(StringListKey& other)
{
    return getHandler().callString(Segment().pushBackString("RPOPLPUSH").pushBackString(getName()).pushBackString(other.getName()));
}

// Get subrange of list (LRANGE).
void
afl::net::redis::StringListKey::getRange(int32_t start, int32_t end, afl::data::StringList_t& list) const
{
    std::auto_ptr<afl::data::Value> val(getHandler().call(Segment().pushBackString("LRANGE").pushBackString(getName()).pushBackInteger(start).pushBackInteger(end)));
    afl::data::Access(val).toStringList(list);
}

// Get all elements of the list (LRANGE).
void
afl::net::redis::StringListKey::getAll(afl::data::StringList_t& list) const
{
    getRange(0, -1, list);
}

