/**
  *  \file afl/io/limiteddatasink.cpp
  *  \brief Class afl::io::LimitedDataSink
  */

#include "afl/io/limiteddatasink.hpp"

afl::io::LimitedDataSink::LimitedDataSink(DataSink& peer, Stream::FileSize_t limit)
    : DataSink(),
      m_peer(peer),
      m_limit(limit)
{ }

afl::io::LimitedDataSink::~LimitedDataSink()
{ }

bool
afl::io::LimitedDataSink::handleData(const String_t& name, afl::base::ConstBytes_t& data)
{
    afl::base::ConstBytes_t peerData(data.splitUpdate(m_limit));
    m_peer.handleData(name, peerData);
    return m_limit == 0;
}
