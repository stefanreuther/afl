/**
  *  \file afl/io/nullstream.cpp
  *  \brief Class afl::io::NullStream
  *
  *  \todo This class is derived from MultiplexableStream.
  *  The only reason we do that is for sharing the m_size member.
  *  We can do much better.
  */

#include "afl/io/nullstream.hpp"
#include "afl/io/filemapping.hpp"

afl::io::NullStream::NullStream()
    : MultiplexableStream(),
      m_pos(0),
      m_size(0)
{ }

afl::io::NullStream::~NullStream()
{ }

size_t
afl::io::NullStream::read(Bytes_t /*m*/)
{
    return 0;
}

size_t
afl::io::NullStream::write(ConstBytes_t m)
{
    m_pos += m.size();
    if (m_pos > m_size) {
        m_size = m_pos;
    }
    return m.size();
}

void
afl::io::NullStream::flush()
{ }

void
afl::io::NullStream::setPos(FileSize_t pos)
{
    m_pos = pos;
}

afl::io::Stream::FileSize_t
afl::io::NullStream::getPos()
{
    return m_pos;
}

afl::io::Stream::FileSize_t
afl::io::NullStream::getSize()
{
    return m_size;
}

uint32_t
afl::io::NullStream::getCapabilities()
{
    return CanWrite | CanSeek;
}

String_t
afl::io::NullStream::getName()
{
    return "<null>";
}

afl::base::Ptr<afl::io::FileMapping>
afl::io::NullStream::createFileMapping(FileSize_t /*limit*/)
{
    return 0;
}
