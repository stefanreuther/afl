/**
  *  \file afl/io/constmemorystream.cpp
  *  \brief Class afl::io::ConstMemoryStream
  */

#include "afl/io/constmemorystream.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/string/messages.hpp"
#include "afl/io/filemapping.hpp"

afl::io::ConstMemoryStream::ConstMemoryStream(ConstBytes_t mem)
    : m_mem(mem),
      m_pos(0)
{ }

afl::io::ConstMemoryStream::~ConstMemoryStream()
{ }

size_t
afl::io::ConstMemoryStream::read(Bytes_t m)
{
    size_t amount = m.copyFrom(m_mem.subrange(m_pos, m.size())).size();
    m_pos += amount;
    return amount;
}

size_t
afl::io::ConstMemoryStream::write(ConstBytes_t /*m*/)
{
    throw afl::except::FileProblemException(*this, afl::string::Messages::cannotWrite());
}

void
afl::io::ConstMemoryStream::flush()
{ }

void
afl::io::ConstMemoryStream::setPos(FileSize_t pos)
{
    if (size_t(pos) != pos) {
        throw afl::except::FileProblemException(*this, afl::string::Messages::rangeError());
    }
    m_pos = size_t(pos);
}

afl::io::ConstMemoryStream::FileSize_t
afl::io::ConstMemoryStream::getPos()
{
    return m_pos;
}

afl::io::ConstMemoryStream::FileSize_t
afl::io::ConstMemoryStream::getSize()
{
    return m_mem.size();
}

uint32_t
afl::io::ConstMemoryStream::getCapabilities()
{
    return CanRead | CanSeek;
}

String_t
afl::io::ConstMemoryStream::getName()
{
    return "<memory>";
}

afl::base::Ref<afl::io::Stream>
afl::io::ConstMemoryStream::createChild(uint32_t /*flags*/)
{
    return *new ConstMemoryStream(m_mem);
}

afl::base::Ptr<afl::io::FileMapping>
afl::io::ConstMemoryStream::createFileMapping(FileSize_t /*limit*/)
{
    return 0;
}
