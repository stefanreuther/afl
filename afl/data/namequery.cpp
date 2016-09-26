/**
  *  \file afl/data/namequery.cpp
  *  \brief Class afl::data::NameQuery
  */

#include "afl/data/namequery.hpp"

const size_t afl::data::NameQuery::HASH_MAX;

namespace {
    /*
     *  Hashing using a simple off-the-hat hash function
     */
    static uint16_t hash(const afl::string::ConstStringMemory_t& name)
    {
        // Caching the string data helps the compiler generate a tighter inner loop.
        // (about 2% percent total speed on my computer)
        const size_t size = name.size();
        const char*const data = name.unsafeData();

        size_t h = size;
        size_t limit = size;
        if (limit > 5) {
            limit = 5;
        }
        for (size_t i = 0; i < limit; ++i) {
            h += uint8_t(data[i]);
            h += uint8_t(data[size-i-1]);
        }
        return h % afl::data::NameQuery::HASH_MAX;
    }
}

afl::data::NameQuery::NameQuery(const String_t& str)
    : m_text(afl::string::toMemory(str)),
      m_hashCode(::hash(m_text))
{ }

afl::data::NameQuery::NameQuery(const char* str)
    : m_text(afl::string::toMemory(str)),
      m_hashCode(::hash(m_text))
{ }

afl::data::NameQuery::NameQuery(const NameQuery& other, size_t skip)
    : m_text(other.m_text.subrange(skip)),
      m_hashCode(::hash(m_text))
{ }
