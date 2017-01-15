/**
  *  \file afl/net/redis/sortoperation.cpp
  *  \brief Class afl::net::redis::SortOperation
  */

#include <memory>
#include "afl/net/redis/sortoperation.hpp"
#include "afl/data/access.hpp"
#include "afl/net/redis/field.hpp"
#include "afl/net/redis/key.hpp"
#include "afl/net/redis/listkey.hpp"

// Constructor.
afl::net::redis::SortOperation::SortOperation(const Key& key)
    : m_ch(key.getHandler()),
      m_request(new afl::data::Segment())
{
    m_request->pushBackString("SORT").pushBackString(key.getName());
}

// Destructor.
afl::net::redis::SortOperation::~SortOperation()
{ }

// Define sort key.
afl::net::redis::SortOperation&
afl::net::redis::SortOperation::by(String_t pattern)
{
    m_request->pushBackString("BY").pushBackString(pattern);
    return *this;
}

// Define sort key, given a key template.
afl::net::redis::SortOperation&
afl::net::redis::SortOperation::by(const Key& key)
{
    return by(key.getName());
}

// Define sort key, given a hash field template.
afl::net::redis::SortOperation&
afl::net::redis::SortOperation::by(const Field& field)
{
    return by(field.getHash().getName() + "->" + field.getName());
}

// Request origin value return.
afl::net::redis::SortOperation&
afl::net::redis::SortOperation::get()
{
    m_request->pushBackString("GET").pushBackString("#");
    return *this;
}

// Request value return.
afl::net::redis::SortOperation&
afl::net::redis::SortOperation::get(String_t pattern)
{
    m_request->pushBackString("GET").pushBackString(pattern);
    return *this;
}

// Request key value return.
afl::net::redis::SortOperation&
afl::net::redis::SortOperation::get(const Key& key)
{
    return get(key.getName());
}

// Request field value return.
afl::net::redis::SortOperation&
afl::net::redis::SortOperation::get(const Field& field)
{
    return get(field.getHash().getName() + "->" + field.getName());
}

// Limit output.
afl::net::redis::SortOperation&
afl::net::redis::SortOperation::limit(int32_t start, int32_t count)
{
    m_request->pushBackString("LIMIT").pushBackInteger(start).pushBackInteger(count);
    return *this;
}

// Enable lexicographical sort.
afl::net::redis::SortOperation&
afl::net::redis::SortOperation::sortLexicographical()
{
    m_request->pushBackString("ALPHA");
    return *this;
}

// Enable reversed (descending) sort.
afl::net::redis::SortOperation&
afl::net::redis::SortOperation::sortReversed()
{
    m_request->pushBackString("DESC");
    return *this;
}

// Disable sort.
afl::net::redis::SortOperation&
afl::net::redis::SortOperation::sortDisable()
{
    return by("nosort");
}

// Get result into a StringList_t.
void
afl::net::redis::SortOperation::getResult(afl::data::StringList_t& out)
{
    std::auto_ptr<afl::data::Value> p(getResult());
    afl::data::Access(p).toStringList(out);
}

// Get result into an IntegerList_t.
void
afl::net::redis::SortOperation::getResult(afl::data::IntegerList_t& out)
{
    std::auto_ptr<afl::data::Value> p(getResult());
    afl::data::Access(p).toIntegerList(out);
}

// Get raw result.
afl::data::Value*
afl::net::redis::SortOperation::getResult()
{
    return m_ch.call(*m_request);
}

// Store result into a ListKey.
void
afl::net::redis::SortOperation::storeResult(ListKey out)
{
    m_request->pushBackString("STORE").pushBackString(out.getName());
    m_ch.callVoid(*m_request);
}
