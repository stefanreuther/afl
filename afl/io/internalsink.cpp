/**
  *  \file afl/io/internalsink.cpp
  *  \brief Class afl::io::InternalSink
  */

#include "afl/io/internalsink.hpp"

afl::io::InternalSink::InternalSink()
    : DataSink(),
      m_data()
{ }

afl::io::InternalSink::~InternalSink()
{ }

bool
afl::io::InternalSink::handleData(const String_t& /*name*/, afl::base::ConstBytes_t& data)
{
    m_data.append(data);
    return false;
}

afl::base::ConstBytes_t
afl::io::InternalSink::getContent() const
{
    return m_data;  // implicitly converted
}

void
afl::io::InternalSink::clear()
{
    m_data.reset();
}
