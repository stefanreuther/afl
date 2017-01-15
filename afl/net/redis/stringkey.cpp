/**
  *  \file afl/net/redis/stringkey.cpp
  *  \brief Class afl::net::redis::StringKey
  */

#include "afl/net/redis/stringkey.hpp"

namespace {
    /** Maximum size of a string.
        Reference: http://redis.io/topics/data-types */
    const size_t MAX_SIZE = 512UL * 1024 * 1024;
}

using afl::data::Segment;

// Constructor.
afl::net::redis::StringKey::StringKey(CommandHandler& ch, String_t name)
    : Key(ch, name)
{ }

// Append text (APPEND).
void
afl::net::redis::StringKey::append(const String_t& text)
{
    getHandler().callVoid(Segment().pushBackString("APPEND").pushBackString(getName()).pushBackString(text));
}

// Get current value (GET).
String_t
afl::net::redis::StringKey::get() const
{
    return getHandler().callString(Segment().pushBackString("GET").pushBackString(getName()));
}

String_t
afl::net::redis::StringKey::substr(std::size_t start, std::size_t length) const
{
    if (start < MAX_SIZE && length > 0) {
        // Limit length.
        if (length > MAX_SIZE) {
            length = MAX_SIZE;
        }

        // Now, both start and length are at most 512M, so the conversion to int32_t cannot overflow.
        return getHandler().callString(Segment().pushBackString("GETRANGE").pushBackString(getName())
                                       .pushBackInteger(static_cast<int32_t>(start))
                                       .pushBackInteger(static_cast<int32_t>(start+length-1)));
    } else {
        return String_t();
    }
}

String_t
afl::net::redis::StringKey::replaceBy(const String_t& newValue)
{
    return getHandler().callString(Segment().pushBackString("GETSET").pushBackString(getName()).pushBackString(newValue));
}

void
afl::net::redis::StringKey::set(const String_t& newValue)
{
    getHandler().callVoid(Segment().pushBackString("SET").pushBackString(getName()).pushBackString(newValue));
}

bool
afl::net::redis::StringKey::setUnique(const String_t& newValue)
{
    return getHandler().callInt(Segment().pushBackString("SETNX").pushBackString(getName()).pushBackString(newValue));
}

size_t
afl::net::redis::StringKey::size() const
{
    int32_t n = getHandler().callInt(Segment().pushBackString("STRLEN").pushBackString(getName()));
    if (n < 0 || static_cast<int32_t>(static_cast<size_t>(n)) != n) {
        // Value is out of range and cannot be used as size
        return 0;
    } else {
        // Value is valid
        return static_cast<size_t>(n);
    }
}
