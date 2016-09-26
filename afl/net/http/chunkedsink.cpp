/**
  *  \file afl/net/http/chunkedsink.cpp
  *  \brief Class afl::net::http::ChunkedSink
  */

#include "afl/net/http/chunkedsink.hpp"

afl::net::http::ChunkedSink::ChunkedSink(afl::io::DataSink& peer)
    : afl::io::DataSink(),
      m_state(ChunkSize),
      m_peer(peer),
      m_size(0)
{ }

afl::net::http::ChunkedSink::~ChunkedSink()
{ }

bool
afl::net::http::ChunkedSink::handleData(const String_t& name, afl::base::ConstBytes_t& data)
{
    while (m_state != Final && data.size() != 0) {
        switch (m_state) {
         case ChunkSize:
            // Read chunk size.
            // "123\r\n" (just a chunk size, continue with payload or trailer)
            // "123;bla\r\n" (chunk size + extension)
            while (const uint8_t* pc = data.eat()) {
                if (*pc >= '0' && *pc <= '9') {
                    m_size = 16*m_size + (*pc - '0');
                } else if (*pc >= 'A' && *pc <= 'F') {
                    m_size = 16*m_size + (*pc - 'A' + 10);
                } else if (*pc >= 'a' && *pc <= 'f') {
                    m_size = 16*m_size + (*pc - 'a' + 10);
                } else if (*pc == ';') {
                    m_state = Extension;
                    break;
                } else if (*pc == '\n') {
                    m_state = m_size == 0 ? Trailer : Payload;
                    break;
                } else {
                    // must be CR; otherwise, it's an error. Just ignore it.
                }
            }
            break;

         case Extension:
            // Read extension after chunk size
            // (skip anything that is not a LF)
            while (const uint8_t* pc = data.eat()) {
                if (*pc == '\n') {
                    m_state = m_size == 0 ? Trailer : Payload;
                    break;
                }
            }
            break;

         case Payload: {
            // Read payload
            size_t n = data.size();
            if (n > m_size) {
                n = size_t(m_size);
            }
            m_size -= n;
            afl::base::ConstBytes_t payload(data.split(n));
            m_peer.handleData(name, payload);
            if (m_size == 0) {
                m_state = PayloadEnd;
            }
            break;
         }

         case PayloadEnd:
            // Read CRLF after payload
            // (ignore anything that is not a LF)
            while (const uint8_t* pc = data.eat()) {
                if (*pc == '\n') {
                    m_state = ChunkSize;
                    break;
                }
            }
            break;

         case Trailer:
            // Read trailer. Trailer ends with a blank line, i.e. just CRLF.
            // If we see anything else, it's a trailer "header" field; see TrailerHeader.
            while (const uint8_t* pc = data.eat()) {
                if (*pc == '\r') {
                    // ignore
                } else if (*pc == '\n') {
                    // ok!
                    m_state = Final;
                    break;
                } else {
                    m_state = TrailerHeader;
                    break;
                }
            }
            break;

         case TrailerHeader:
            // Trailer "header" field. Skip whole line until LF.
            while (const uint8_t* pc = data.eat()) {
                if (*pc == '\n') {
                    m_state = Trailer;
                    break;
                }
            }
            break;

         case Final:
            // Final state.
            break;
        }
    }
    return m_state == Final;
}
