/**
  *  \file afl/io/bufferedsink.cpp
  *  \brief Class afl::io::BufferedSink
  */

#include <cassert>
#include "afl/io/bufferedsink.hpp"

afl::io::BufferedSink::BufferedSink(DataSink& sink)
    : m_sink(sink),
      m_lastName(),
      m_buffer(m_rawBuffer),
      m_bufferFill(0)
{ }

afl::io::BufferedSink::~BufferedSink()
{
    flush();
}

bool
afl::io::BufferedSink::handleData(const String_t& name, afl::base::ConstBytes_t& data)
{
    m_lastName = name;
    while (!data.empty()) {
        if (m_bufferFill == 0 && data.size() >= m_buffer.size()) {
            // Write directly
            m_sink.handleData(name, data);
            data.reset();
        } else {
            // Write through buffer
            size_t copied = m_buffer.subrange(m_bufferFill).copyFrom(data).size();
            assert(copied != 0);
            data.split(copied);
            m_bufferFill += copied;
            if (m_bufferFill >= m_buffer.size()) {
                flush();
            }
        }
    }
    return false;
}

void
afl::io::BufferedSink::flush()
{
    if (m_bufferFill != 0) {
        // Write out data
        afl::base::ConstBytes_t tmp(m_buffer);
        tmp.trim(m_bufferFill);
        m_sink.handleData(m_lastName, tmp);

        // Reset state
        m_buffer = m_rawBuffer;
        m_bufferFill = 0;
    }
}
