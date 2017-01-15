/**
  *  \file afl/net/redis/setkey.cpp
  *  \brief Class afl::net::redis::SetKey
  */

#include "afl/net/redis/setkey.hpp"
#include "afl/net/redis/sortoperation.hpp"

using afl::data::Segment;

// Constructor.
afl::net::redis::SetKey::SetKey(CommandHandler& ch, const String_t& name)
    : Key(ch, name)
{ }

// Get number of elements in set (SCARD).
int32_t
afl::net::redis::SetKey::size() const
{
    return getHandler().callInt(Segment().pushBackString("SCARD").pushBackString(getName()));
}

// Check emptiness (SCARD).
bool
afl::net::redis::SetKey::empty() const
{
    return size() == 0;
}

// Sort operation (SORT).
afl::net::redis::SortOperation
afl::net::redis::SetKey::sort() const
{
    return SortOperation(*this);
}
