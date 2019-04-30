/**
  *  \file afl/data/hash.cpp
  *  \brief Class afl::data::Hash
  */

#include "afl/data/hash.hpp"

afl::data::Hash::Ref_t
afl::data::Hash::create()
{
    return *new Hash();
}

afl::data::Hash::Ref_t
afl::data::Hash::create(NameMap& keys, Segment& values)
{
    Ref_t hash = *new Hash();
    hash->m_keys.swap(keys);
    hash->m_values.swap(values);
    return *hash;
}

inline
afl::data::Hash::Hash()
    : m_keys(),
      m_values()
{ }

afl::data::Hash::~Hash()
{ }

void
afl::data::Hash::set(const String_t& key, const Value* value)
{
    setNew(key, Value::cloneOf(value));
}

void
afl::data::Hash::setNew(const String_t& key, Value* value)
{
    NameMap::Index_t index;
    try {
        index = m_keys.addMaybe(key);
    }
    catch (...) {
        delete value;
        throw;
    }

    m_values.setNew(index, value);
}

afl::data::Value*
afl::data::Hash::get(const String_t& key) const
{
    return m_values[m_keys.getIndexByName(key)];
}

bool
afl::data::Hash::getIndexByKey(const NameQuery& key, Index_t& index) const
{
    Index_t i = m_keys.getIndexByName(key);
    if (i != NameMap::nil) {
        index = i;
        return true;
    } else {
        return false;
    }
}

void
afl::data::Hash::setValueByIndex(Index_t index, const Value* value)
{
    m_values.set(index, value);
}

void
afl::data::Hash::setValueByIndexNew(Index_t index, Value* value)
{
    m_values.setNew(index, value);
}

afl::data::Value*
afl::data::Hash::getValueByIndex(Index_t index) const
{
    return m_values[index];
}

const afl::data::NameMap&
afl::data::Hash::getKeys() const
{
    return m_keys;
}

const afl::data::Segment&
afl::data::Hash::getValues() const
{
    return m_values;
}
