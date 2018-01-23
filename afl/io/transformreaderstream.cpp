/**
  *  \file afl/io/transformreaderstream.cpp
  *  \brief Class afl::io::TransformReaderStream
  */

#include "afl/io/transformreaderstream.hpp"
#include "afl/string/messages.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/io/filemapping.hpp"
#include "afl/io/transform.hpp"

afl::io::TransformReaderStream::TransformReaderStream(Stream& stream, Transform& transform)
    : m_stream(stream),
      m_transform(transform),
      m_input(),
      m_numBytesRead(0),
      m_flushed(false)
{ }

afl::io::TransformReaderStream::~TransformReaderStream()
{ }

size_t
afl::io::TransformReaderStream::read(Bytes_t m)
{
    size_t did = 0;
    while (1) {
        // Transform
        Bytes_t out = m.subrange(did);
        m_transform.transform(m_input, out);
        did += out.size();

        // Update file position at this place, so if the stream throws, we're not affected.
        m_numBytesRead += out.size();

        if (did >= m.size()) {
            break;
        }

        // Process buffer
        if (m_input.empty()) {
            // Input is empty.
            // If we already flushed, that mean's the file ends now.
            if (m_flushed) {
                break;
            }

            // Didn't flush, so there's a chance we can re-fill.
            m_input = m_inputBuffer;
            m_input.trim(m_stream.read(m_inputBuffer));
            if (m_input.empty()) {
                // No, flush
                m_transform.flush();
                m_flushed = true;
            }
        }
    }
    return did;
}

size_t
afl::io::TransformReaderStream::write(ConstBytes_t /*m*/)
{
    throw afl::except::FileProblemException(*this, afl::string::Messages::cannotWrite());
}

void
afl::io::TransformReaderStream::flush()
{ }

void
afl::io::TransformReaderStream::setPos(FileSize_t pos)
{
    if (pos != m_numBytesRead) {
        throw afl::except::FileProblemException(*this, afl::string::Messages::invalidOperation());
    }
}

afl::io::Stream::FileSize_t
afl::io::TransformReaderStream::getPos()
{
    return m_numBytesRead;
}

afl::io::Stream::FileSize_t
afl::io::TransformReaderStream::getSize()
{
    return -1;
}

uint32_t
afl::io::TransformReaderStream::getCapabilities()
{
    return CanRead;
}

String_t
afl::io::TransformReaderStream::getName()
{
    return m_stream.getName();
}

afl::base::Ptr<afl::io::FileMapping>
afl::io::TransformReaderStream::createFileMapping(FileSize_t /*limit*/)
{
    return 0;
}
