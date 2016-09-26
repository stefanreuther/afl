/**
  *  \file afl/net/redis/subtree.cpp
  *  \brief Class afl::net::redis::Subtree
  */

#include <memory>
#include "afl/net/redis/subtree.hpp"
#include "afl/data/segment.hpp"
#include "afl/data/access.hpp"
#include "afl/string/format.hpp"

using afl::data::Segment;

namespace {
    const char DELIMITER[] = ":";
}

// Constructor.
afl::net::redis::Subtree::Subtree(CommandHandler& ch, const String_t& name)
    : m_ch(ch),
      m_name(name)
{ }

// Destructor.
afl::net::redis::Subtree::~Subtree()
{ }

// Get names of all subkeys within this subtree (KEYS).
void
afl::net::redis::Subtree::getKeyNames(afl::data::StringList_t& keyNames)
{
    const String_t& name = getName();
    std::auto_ptr<afl::data::Value> val(getHandler().call(Segment().pushBack("KEYS").pushBack(name + "*")));
    afl::data::Access a(val);
    for (size_t i = 0, n = a.getArraySize(); i < n; ++i) {
        String_t key = a[i].toString();
        if (key.size() >= name.size() && key.compare(0, name.size(), name) == 0) {
            keyNames.push_back(key.substr(name.size()));
        }
    }
}

// Get access to a hash key.
afl::net::redis::HashKey
afl::net::redis::Subtree::hashKey(const String_t& name) const
{
    return HashKey(getHandler(), getName() + name);
}

// Get access to a integer key.
afl::net::redis::IntegerKey
afl::net::redis::Subtree::intKey(const String_t& name) const
{
    return IntegerKey(getHandler(), getName() + name);
}

// Get access to a string key.
afl::net::redis::StringKey
afl::net::redis::Subtree::stringKey(const String_t& name) const
{
    return StringKey(getHandler(), getName() + name);
}

// Get access to an integer list key.
afl::net::redis::IntegerListKey
afl::net::redis::Subtree::intListKey(const String_t& name) const
{
    return IntegerListKey(getHandler(), getName() + name);
}

// Get access to a string list key.
afl::net::redis::StringListKey
afl::net::redis::Subtree::stringListKey(const String_t& name) const
{
    return StringListKey(getHandler(), getName() + name);
}

// Get access to an integer set key.
afl::net::redis::IntegerSetKey
afl::net::redis::Subtree::intSetKey(const String_t& name) const
{
    return IntegerSetKey(getHandler(), getName() + name);
}

// Get access to a string set key.
afl::net::redis::StringSetKey
afl::net::redis::Subtree::stringSetKey(const String_t& name) const
{
    return StringSetKey(getHandler(), getName() + name);
}

// Get access to untyped key.
afl::net::redis::Key
afl::net::redis::Subtree::key(const String_t& name) const
{
    return Key(getHandler(), getName() + name);
}

// Get access to a subtree.
afl::net::redis::Subtree
afl::net::redis::Subtree::subtree(const String_t& name) const
{
    return Subtree(getHandler(), getName() + name + DELIMITER);
}

// Get access to a subtree, integer name.
afl::net::redis::Subtree
afl::net::redis::Subtree::subtree(int32_t name) const
{
    return subtree(afl::string::Format("%d", name));
}

// Get name (prefix) of this subtree.
const String_t&
afl::net::redis::Subtree::getName() const
{
    return m_name;
}

afl::net::CommandHandler&
afl::net::redis::Subtree::getHandler() const
{
    return m_ch;
}
