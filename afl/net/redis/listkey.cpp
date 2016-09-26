/**
  *  \file afl/net/redis/listkey.cpp
  *  \brief Class afl::net::redis::ListKey
  */

#include "afl/net/redis/listkey.hpp"
#include "afl/net/redis/sortoperation.hpp"

using afl::data::Segment;

// Constructor.
afl::net::redis::ListKey::ListKey(CommandHandler& ch, String_t name)
    : Key(ch, name)
{ }

// Get number of elements in list (LLEN).
int32_t
afl::net::redis::ListKey::size() const
{
    return getHandler().callInt(Segment().pushBack("LLEN").pushBack(getName()));
}

// Check emptiness (LLEN).
bool
afl::net::redis::ListKey::empty() const
{
    return size() == 0;
}

// Trim list to a given range (LTRIM).
void
afl::net::redis::ListKey::trimToRange(int32_t start, int32_t end)
{
    getHandler().callVoid(Segment().pushBack("LTRIM").pushBack(getName()).pushBack(start).pushBack(end));
}

// Sort operation (SORT).
afl::net::redis::SortOperation
afl::net::redis::ListKey::sort() const
{
    return SortOperation(*this);
}
