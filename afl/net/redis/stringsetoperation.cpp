/**
  *  \file afl/net/redis/stringsetoperation.cpp
  *  \brief Class afl::net::redis::StringSetOperation
  */

#include <memory>
#include "afl/net/redis/stringsetoperation.hpp"
#include "afl/data/stringvalue.hpp"
#include "afl/data/access.hpp"
#include "afl/net/redis/stringsetkey.hpp"

// Constructor.
afl::net::redis::StringSetOperation::StringSetOperation(const char op[], const StringSetKey& a, const StringSetKey& b)
    : m_ch(a.getHandler()),
      m_request(new afl::data::Segment())
{
    m_request->pushBackString(op).pushBackString(a.getName()).pushBackString(b.getName());
}

// Add additional set.
afl::net::redis::StringSetOperation&
afl::net::redis::StringSetOperation::andAlso(StringSetKey x)
{
    m_request->pushBackString(x.getName());
    return *this;
}

// Get result of set operation.
void
afl::net::redis::StringSetOperation::getAll(afl::data::StringList_t& list)
{
    std::auto_ptr<afl::data::Value> result(m_ch.call(*m_request));
    afl::data::Access(result).toStringList(list);
}

// Store result of operation into new set.
void
afl::net::redis::StringSetOperation::storeTo(StringSetKey other)
{
    // m_request is [<op>,a,b,c].
    // We need      [<op>STORE,other,a,b,c].
    m_request->setNew(0, new afl::data::StringValue(afl::data::Access((*m_request)[0]).toString() + "STORE"));
    m_request->insertNew(1, new afl::data::StringValue(other.getName()));
    m_ch.callVoid(*m_request);
}
