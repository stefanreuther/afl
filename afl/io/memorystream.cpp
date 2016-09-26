/**
  *  \file afl/io/memorystream.cpp
  *  \brief Class afl::io::MemoryStream
  */

#include "afl/io/memorystream.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/string/messages.hpp"
#include "afl/io/filemapping.hpp"

afl::io::MemoryStream::MemoryStream(Bytes_t mem)
    : m_mem(mem),
      m_pos(0)
{ }

afl::io::MemoryStream::~MemoryStream()
{ }

size_t
afl::io::MemoryStream::read(Bytes_t m)
{
    size_t amount = m.copyFrom(m_mem.subrange(m_pos, m.size())).size();
    m_pos += amount;
    return amount;
}

size_t
afl::io::MemoryStream::write(ConstBytes_t m)
{
    size_t amount = m_mem.subrange(m_pos, m.size()).copyFrom(m).size();
    m_pos += amount;
    return amount;
}

void
afl::io::MemoryStream::flush()
{ }

void
afl::io::MemoryStream::setPos(FileSize_t pos)
{
    if (size_t(pos) != pos) {
        throw afl::except::FileProblemException(*this, afl::string::Messages::rangeError());
    }
    m_pos = size_t(pos);
}

afl::io::MemoryStream::FileSize_t
afl::io::MemoryStream::getPos()
{
    return m_pos;
}

afl::io::MemoryStream::FileSize_t
afl::io::MemoryStream::getSize()
{
    return m_mem.size();
}

uint32_t
afl::io::MemoryStream::getCapabilities()
{
    return CanRead | CanWrite | CanSeek;
}

String_t
afl::io::MemoryStream::getName()
{
    return "<memory>";
}

afl::base::Ptr<afl::io::FileMapping>
afl::io::MemoryStream::createFileMapping(FileSize_t /*limit*/)
{
    return 0;
}
