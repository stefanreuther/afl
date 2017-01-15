/**
  *  \file afl/net/redis/key.cpp
  *  \brief Class afl::net::redis::Key
  */

#include "afl/net/redis/key.hpp"
#include "afl/data/segment.hpp"

using afl::data::Segment;

// Constructor
afl::net::redis::Key::Key(CommandHandler& ch, const String_t& name)
    : m_ch(ch),
      m_name(name)
{ }

// Destructor.
afl::net::redis::Key::~Key()
{ }

// Check for existence (EXISTS).
bool
afl::net::redis::Key::exists() const
{
    return m_ch.callInt(Segment().pushBackString("EXISTS").pushBackString(m_name)) != 0;
}

// Remove this key from the database (DEL).
void
afl::net::redis::Key::remove()
{
    m_ch.callVoid(Segment().pushBackString("DEL").pushBackString(m_name));
}

// Rename this key (RENAME).
void
afl::net::redis::Key::renameTo(String_t newName)
{
    m_ch.callVoid(Segment().pushBackString("RENAME").pushBackString(m_name).pushBackString(newName));
    m_name = newName;
}

// Rename this key to non-existant name (RENAMENX).
bool
afl::net::redis::Key::renameToUnique(String_t newName)
{
    const bool result = m_ch.callInt(Segment().pushBackString("RENAMENX").pushBackString(m_name).pushBackString(newName));
    if (result) {
        m_name = newName;
    }
    return result;
}

// Get type of key (TYPE).
afl::net::redis::Key::Type
afl::net::redis::Key::getType() const
{
    const String_t result = m_ch.callString(Segment().pushBackString("TYPE").pushBackString(m_name));
    if (result == "none") {
        return None;
    } else if (result == "string") {
        return String;
    } else if (result == "list") {
        return List;
    } else if (result == "set") {
        return Set;
    } else if (result == "zset") {
        return ZSet;
    } else if (result == "hash") {
        return Hash;
    } else {
        return Unknown;
    }
}

// Get name of this key.
const String_t&
afl::net::redis::Key::getName() const
{
    return m_name;
}

// Get command client used for this key.
afl::net::CommandHandler&
afl::net::redis::Key::getHandler() const
{
    return m_ch;
}
