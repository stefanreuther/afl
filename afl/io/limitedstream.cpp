/**
  *  \file afl/io/limitedstream.cpp
  *  \brief Class afl::io::LimitedStream
  */

#include "afl/io/limitedstream.hpp"
#include "afl/string/messages.hpp"
#include "afl/except/filesystemexception.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/io/filemapping.hpp"

const afl::io::LimitedStream::FileSize_t afl::io::LimitedStream::nil;


// Check whether initial position was set. In this case, we can seek.
inline bool
afl::io::LimitedStream::hasPosition() const
{
    return m_origin != nil;
}

// Check whether length limit was set. In this case, we need to track bytes read and/or position.
inline bool
afl::io::LimitedStream::hasLength() const
{
    return m_length != nil;
}


// Constructor.
afl::io::LimitedStream::LimitedStream(afl::base::Ref<Stream> parent, FileSize_t origin, FileSize_t length)
    : m_parent(parent),
      m_origin(origin),
      m_length(length),
      m_position(0)
{
    // Go to beginning of segment
    if (hasPosition()) {
        setPos(0);
    }
}

afl::io::LimitedStream::~LimitedStream()
{ }

size_t
afl::io::LimitedStream::read(Bytes_t m)
{
    // Limit to length
    if (hasLength() && m.size() > m_length - m_position) {
        m.trim(size_t(m_length - m_position));
    }

    // Read
    try {
        size_t result = m_parent->read(m);
        m_position += result;
        return result;
    }
    catch (...) {
        fixupPosition(m.size());
        throw;
    }
}

size_t
afl::io::LimitedStream::write(ConstBytes_t m)
{
    // Limit to length
    if (hasLength() && m.size() > m_length - m_position) {
        m.trim(size_t(m_length - m_position));
    }

    // Write
    try {
        size_t result = m_parent->write(m);
        m_position += result;
        return result;
    }
    catch (...) {
        fixupPosition(m.size());
        throw;
    }
}

void
afl::io::LimitedStream::flush()
{
    m_parent->flush();
}

void
afl::io::LimitedStream::setPos(FileSize_t pos)
{
    if (!hasPosition()) {
        throw afl::except::FileProblemException(*this, afl::string::Messages::invalidOperation());
    } else {
        if (hasLength() && pos > m_length) {
            throw afl::except::FileProblemException(*this, afl::string::Messages::invalidOperation());
        }
        m_parent->setPos(m_origin + pos);
        m_position = pos;
    }
}

afl::io::LimitedStream::FileSize_t
afl::io::LimitedStream::getPos()
{
    return m_position;
}

afl::io::LimitedStream::FileSize_t
afl::io::LimitedStream::getSize()
{
    return m_length;
}

uint32_t
afl::io::LimitedStream::getCapabilities()
{
    uint32_t ops = m_parent->getCapabilities();
    if (!hasPosition()) {
        ops &= ~CanSeek;
    }
    return ops;
}

String_t
afl::io::LimitedStream::getName()
{
    return m_parent->getName();
}

afl::base::Ref<afl::io::Stream>
afl::io::LimitedStream::createChild()
{
    return *new LimitedStream(m_parent->createChild(), m_origin, m_length);
}

afl::base::Ptr<afl::io::FileMapping>
afl::io::LimitedStream::createFileMapping(FileSize_t limit)
{
    if (hasLength() && limit > m_length - m_position) {
        limit = m_length - m_position;
    }
    return m_parent->createFileMapping(limit);
}

// Fix up position after something unexpected happened to the stream during a read/write of n bytes.
void
afl::io::LimitedStream::fixupPosition(size_t n)
{
    if (hasLength()) {
        // Length limiting required
        if (hasPosition()) {
            // Absolute positions are known
            FileSize_t pos = m_parent->getPos();
            if (pos < m_origin) {
                // Error case, parent went before origin
                setPos(0);
            } else if (pos - m_origin > m_length) {
                // Error case, parent went beyond end
                setPos(m_length);
            } else {
                // Regular case
                m_position = pos - m_origin;
            }
        } else {
            // Absolute positions not known. Assume parent has consumed everything before throwing.
            // (Pessimistic assumption to avoid anyone reads across their boundary.)
            m_position += n;
        }
    }
}
