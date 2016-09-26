/**
  *  \file afl/data/namemap.cpp
  *  \brief Class afl::data::NameMap
  */

#include "afl/data/namemap.hpp"

const afl::data::NameMap::Index_t afl::data::NameMap::nil;

afl::data::NameMap::NameMap()
    : m_names(),
      m_hashFirst(NameQuery::HASH_MAX, nil),
      m_hashNext()
{ }

afl::data::NameMap::~NameMap()
{ }

afl::data::NameMap::Index_t
afl::data::NameMap::getIndexByName(const NameQuery& name) const
{
    Index_t i = m_hashFirst[name.getHashCode()];
    while (i != nil && !name.match(m_names[i])) {
        i = m_hashNext[i];
    }
    return i;
}

afl::data::NameMap::Index_t
afl::data::NameMap::add(const Name_t& name)
{
    const size_t h = NameQuery(name).getHashCode();
    const Index_t slot = static_cast<Index_t>(m_names.size());
    m_names.push_back(name);
    m_hashNext.push_back(m_hashFirst[h]);
    m_hashFirst[h] = slot;
    return slot;
}

afl::data::NameMap::Index_t
afl::data::NameMap::addMaybe(const Name_t& name)
{
    Index_t i = getIndexByName(name);
    if (i == nil) {
        i = add(name);
    }
    return i;
}

void
afl::data::NameMap::swap(NameMap& other)
{
    m_names.swap(other.m_names);
    m_hashFirst.swap(other.m_hashFirst);
    m_hashNext.swap(other.m_hashNext);
}
