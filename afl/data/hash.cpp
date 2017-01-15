/**
  *  \file afl/data/hash.cpp
  *  \brief Class afl::data::Hash
  */

#include "afl/data/hash.hpp"

afl::base::Ref<afl::data::Hash>
afl::data::Hash::create()
{
    return *new Hash();
}

afl::base::Ref<afl::data::Hash>
afl::data::Hash::create(NameMap& keys, Segment& values)
{
    afl::base::Ref<afl::data::Hash> hash = *new Hash();
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
afl::data::Hash::set(const String_t& key, Value* value)
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
