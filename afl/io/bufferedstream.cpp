/**
  *  \file afl/io/bufferedstream.cpp
  *  \brief Class afl::io::BufferedStream
  */

#include <algorithm>
#include <cassert>
#include "afl/io/bufferedstream.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/string/messages.hpp"
#include "afl/io/filemapping.hpp"

afl::io::BufferedStream::BufferedStream(Stream& s)
    : MultiplexableStream(),
      m_mode(Neutral),
      m_stream(s),
      m_buffer(),
      m_bufferFill(0)
{ }

afl::io::BufferedStream::~BufferedStream()
{
    try {
        flush();
    }
    catch (...) {
        // There's no way we could report this error,
        // but letting the exception pass would abort the program.
        // So all we can do is ignore it.
    }
}

size_t
afl::io::BufferedStream::read(Bytes_t m)
{
    // Quick, simple, dirty
    size_t did = 0;
    uint8_t* pout;
    const uint8_t* pin;
    while ((pout = m.eat()) != 0 && (pin = readByte()) != 0) {
        *pout = *pin;
        ++did;
    }
    return did;
}

size_t
afl::io::BufferedStream::write(ConstBytes_t m)
{
    setMode(Writing);
    size_t did = 0;
    while (!m.empty()) {
        if (m_bufferFill == 0 && m.size() >= m_buffer.size()) {
            // More than one buffer full: try to write directly.
            size_t now = m_stream.write(m);
            if (now == 0) {
                throw afl::except::FileProblemException(*this, afl::string::Messages::cannotWrite());
            }
            did += now;
            m.split(now);
        } else {
            // Write through buffer
            size_t copied = m_buffer.subrange(m_bufferFill).copyFrom(m).size();
            assert(copied != 0);
            m.split(copied);
            m_bufferFill += copied;
            did += copied;
            if (m_bufferFill >= m_buffer.size()) {
                flushBuffer();
            }
        }
    }
    return did;
}

void
afl::io::BufferedStream::setPos(FileSize_t pos)
{
    setMode(Neutral);
    m_stream.setPos(pos);
}

afl::io::BufferedStream::FileSize_t
afl::io::BufferedStream::getPos()
{
    Stream::FileSize_t p = m_stream.getPos();
    switch (m_mode) {
     case Reading:
        p -= m_buffer.size();
        break;
     case Writing:
        p += m_bufferFill;
        break;
     case Neutral:
        break;
    }
    return p;
}

afl::io::BufferedStream::FileSize_t
afl::io::BufferedStream::getSize()
{
    Stream::FileSize_t p = m_stream.getSize();
    switch (m_mode) {
     case Writing:
        p = std::max(p, m_stream.getPos() + m_bufferFill);
        break;
     case Reading:
     case Neutral:
        break;
    }
    return p;
}

uint32_t
afl::io::BufferedStream::getCapabilities()
{
    return m_stream.getCapabilities();
}

String_t
afl::io::BufferedStream::getName()
{
    return m_stream.getName();
}

afl::base::Ptr<afl::io::FileMapping>
afl::io::BufferedStream::createFileMapping(FileSize_t limit)
{
    // Make sure underlying stream's status agrees with out virtual status
    setMode(Neutral);

    // Create mapping
    return m_stream.createFileMapping(limit);
}

void
afl::io::BufferedStream::flush()
{
    setMode(Neutral);
    m_stream.flush();
}

const uint8_t*
afl::io::BufferedStream::readByte()
{
    setMode(Reading);
    const uint8_t* result = m_buffer.eat();
    if (result == 0) {
        refillBuffer();
        result = m_buffer.eat();
    }
    return result;
}

const uint8_t*
afl::io::BufferedStream::peekByte()
{
    setMode(Reading);
    const uint8_t* result = m_buffer.at(0);
    if (result == 0) {
        refillBuffer();
        result = m_buffer.at(0);
    }
    return result;
}

void
afl::io::BufferedStream::setMode(Mode m)
{
    if (m != m_mode) {
        // Process old mode
        switch (m_mode) {
         case Reading:
            // We are reading, and have likely read too much.
            if (!m_buffer.empty()) {
                m_stream.setPos(m_stream.getPos() - m_buffer.size());
            }
            break;

         case Writing:
            // Just flush the buffer. Make sure the reader doesn't try BOM snooping.
            flushBuffer();
            break;

         case Neutral:
            break;
        }

        // Process new mode
        switch (m) {
         case Writing:
            // We want to write. Check that this is possible.
            // (It is not needed to implement an equivalent
            // check for reading, because setMode(Reading)
            // will normally be followed by a real call to
            // read() which generates the error; for writing,
            // there can be an arbitrarily long time between
            // buffering and actual writing.)
            if (!m_stream.hasCapabilities(Stream::CanWrite)) {
                m_mode = Neutral;
                throw afl::except::FileProblemException(m_stream, afl::string::Messages::cannotWrite());
            }
            m_buffer = m_rawBuffer;
            m_bufferFill = 0;
            break;

         case Reading:
            // Make sure the next read will call refillBuffer()
            m_buffer.reset();
            break;

         case Neutral:
            break;
        }
        m_mode = m;
    }
}

void
afl::io::BufferedStream::refillBuffer()
{
    assert(m_mode == Reading);
    assert(m_buffer.empty());

    m_buffer = m_rawBuffer;
    m_buffer.trim(m_stream.read(m_buffer));
}

void
afl::io::BufferedStream::flushBuffer()
{
    assert(m_mode == Writing);

    if (m_bufferFill != 0) {
        m_buffer.trim(m_bufferFill);
        m_stream.fullWrite(m_buffer);
    }

    m_buffer = m_rawBuffer;
    m_bufferFill = 0;
}
