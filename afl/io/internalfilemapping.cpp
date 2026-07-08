/**
  *  \file afl/io/internalfilemapping.cpp
  *  \brief Class afl::io::InternalFileMapping
  */

#include "afl/io/internalfilemapping.hpp"

// Constructor.
afl::io::InternalFileMapping::InternalFileMapping(Stream& stream, Stream::FileSize_t limit)
    : m_data()
{
    stream.readAll(m_data, limit);
}

// Construct from memory buffer.
afl::io::InternalFileMapping::InternalFileMapping(afl::base::GrowableBytes_t& mem)
    : m_data()
{
    m_data.swap(mem);
}

// Destructor.
afl::io::InternalFileMapping::~InternalFileMapping()
{ }

// Get content of file mapping.
afl::base::ConstBytes_t
afl::io::InternalFileMapping::get() const
{
    return m_data;
}
