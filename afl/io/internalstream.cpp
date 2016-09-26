/**
  *  \file afl/io/internalstream.cpp
  *  \brief Class afl::io::InternalStream
  */

#include <cassert>
#include "afl/io/internalstream.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/string/messages.hpp"
#include "afl/io/filemapping.hpp"

afl::io::InternalStream::InternalStream()
    : MultiplexableStream(),
      m_name("<internal>"),
      m_data(),
      m_pos(0)
{ }

afl::io::InternalStream::~InternalStream()
{ }

// Stream methods:
size_t
afl::io::InternalStream::read(Bytes_t m)
{
    // Copy out subrange
    size_t amount = m.copyFrom(m_data.subrange(m_pos, m.size())).size();
    m_pos += amount;
    return amount;
}

size_t
afl::io::InternalStream::write(ConstBytes_t m)
{
    // Compute new size and check overflow
    size_t end = m_pos + m.size();
    if (end < m_pos) {
        throw afl::except::FileProblemException(*this, afl::string::Messages::rangeError());
    }

    // Resize.
    // If we have unwritten bytes, we must explicitly initialize them (appendN).
    // Bytes we're going to overwrite can be left uninitialized (ensureSize).
    if (m_pos > m_data.size()) {
        m_data.appendN(0, m_pos - m_data.size());
    }
    m_data.ensureSize(end);

    // Copy in
    size_t amount = m_data.subrange(m_pos, m.size()).copyFrom(m).size();
    m_pos += amount;

    // Invariants
    assert(m_pos == end);
    assert(amount == m.size());
    return amount;
}

void
afl::io::InternalStream::flush()
{ }

void
afl::io::InternalStream::setPos(FileSize_t pos)
{
    if (size_t(pos) != pos) {
        throw afl::except::FileProblemException(*this, afl::string::Messages::rangeError());
    }
    m_pos = size_t(pos);
}

afl::io::InternalStream::FileSize_t
afl::io::InternalStream::getPos()
{
    return m_pos;
}

afl::io::InternalStream::FileSize_t
afl::io::InternalStream::getSize()
{
    return m_data.size();
}

uint32_t
afl::io::InternalStream::getCapabilities()
{
    return CanRead | CanWrite | CanSeek;
}

String_t
afl::io::InternalStream::getName()
{
    return m_name;
}

afl::base::Ptr<afl::io::FileMapping>
afl::io::InternalStream::createFileMapping(FileSize_t /*limit*/)
{
    return 0;
}

void
afl::io::InternalStream::setName(const String_t& name)
{
    m_name = name;
}

afl::io::Stream::ConstBytes_t
afl::io::InternalStream::getContent() const
{
    return m_data;  // implicitly converted
}
