/**
  *  \file afl/io/internalfilemapping.cpp
  *  \brief Class afl::io::InternalFileMapping
  */

#include "afl/io/internalfilemapping.hpp"

// Constructor.
afl::io::InternalFileMapping::InternalFileMapping(Stream& stream, Stream::FileSize_t limit)
    : m_data()
{
    init(stream, limit);
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

void
afl::io::InternalFileMapping::init(Stream& stream, Stream::FileSize_t limit)
{
    uint8_t localBuffer[4096];
    while (limit > 0) {
        // Figure out how much to read
        afl::base::Bytes_t toRead(localBuffer);
        if (limit < toRead.size()) {
            toRead.trim(size_t(limit));
        }

        // Read
        size_t got = stream.read(toRead);
        if (got == 0) {
            // File ends.
            break;
        }

        // Update
        m_data.append(toRead.trim(got));
        limit -= got;
    }
}
