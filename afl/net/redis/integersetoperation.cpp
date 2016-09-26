/**
  *  \file afl/net/redis/integersetoperation.cpp
  *  \brief Class afl::net::redis::IntegerSetOperation
  */

#include <memory>
#include "afl/net/redis/integersetoperation.hpp"
#include "afl/data/stringvalue.hpp"
#include "afl/data/access.hpp"
#include "afl/net/redis/integersetkey.hpp"

// Constructor.
afl::net::redis::IntegerSetOperation::IntegerSetOperation(const char op[], const IntegerSetKey& a, const IntegerSetKey& b)
    : m_ch(a.getHandler()),
      m_request(new afl::data::Segment())
{
    m_request->pushBack(op).pushBack(a.getName()).pushBack(b.getName());
}

// Add additional set.
afl::net::redis::IntegerSetOperation&
afl::net::redis::IntegerSetOperation::andAlso(IntegerSetKey x)
{
    m_request->pushBack(x.getName());
    return *this;
}

// Get result of set operation.
void
afl::net::redis::IntegerSetOperation::getAll(afl::data::IntegerList_t& list)
{
    std::auto_ptr<afl::data::Value> result(m_ch.call(*m_request));
    afl::data::Access(result).toIntegerList(list);
}

// Store result of operation into new set.
void
afl::net::redis::IntegerSetOperation::storeTo(IntegerSetKey other)
{
    // m_request is [<op>,a,b,c].
    // We need      [<op>STORE,other,a,b,c].
    m_request->setNew(0, new afl::data::StringValue(afl::data::Access((*m_request)[0]).toString() + "STORE"));
    m_request->insertNew(1, new afl::data::StringValue(other.getName()));
    m_ch.callVoid(*m_request);
}
