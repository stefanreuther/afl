/**
  *  \file afl/net/http/protocolhandler.cpp
  *  \class afl::net::http::ProtocolHandler
  */

#include "afl/net/http/protocolhandler.hpp"
#include "afl/base/types.hpp"
#include "afl/io/limiteddatasink.hpp"
#include "afl/io/stream.hpp"
#include "afl/net/http/chunkedsink.hpp"
#include "afl/net/http/dispatcher.hpp"
#include "afl/net/http/errorresponse.hpp"
#include "afl/net/http/request.hpp"
#include "afl/net/http/response.hpp"
#include "afl/string/parse.hpp"

namespace {
    const int32_t RECEIVE_TIMEOUT = 30000;
    const int32_t SEND_TIMEOUT = 30000;
    const size_t MAX_REQUEST_HEADER_LENGTH = 32768;
}

// Constructor.
afl::net::http::ProtocolHandler::ProtocolHandler(Dispatcher& dispatcher)
    : m_dispatcher(dispatcher),
      m_state(ReadingRequest),
      m_request(new Request()),
      m_response(),
      m_requestHeaderLength(0),
      m_maxRequestHeaderLength(MAX_REQUEST_HEADER_LENGTH),
      m_responseSink(),
      m_responseKeepalive(false),
      m_finishedResponses()
{ }

// Destructor.
afl::net::http::ProtocolHandler::~ProtocolHandler()
{ }

// Get operation to perform.
void
afl::net::http::ProtocolHandler::getOperation(Operation& op)
{
    // Try to send some data
    while (!m_finishedResponses.empty()) {
        op.m_dataToSend = m_finishedResponses.front()->getData();
        if (!op.m_dataToSend.empty()) {
            op.m_close = false;
            op.m_timeToWait = SEND_TIMEOUT;
            return;
        }
        m_finishedResponses.popFront();
    }

    // Could not send data; try receiving
    op.m_dataToSend.reset();
    op.m_close = (m_state == Close);
    op.m_timeToWait = RECEIVE_TIMEOUT;
}

// Advance time.
void
afl::net::http::ProtocolHandler::advanceTime(afl::sys::Timeout_t /*msecs*/)
{ }

// Handle incoming data.
void
afl::net::http::ProtocolHandler::handleData(afl::base::ConstBytes_t bytes)
{
    while (!bytes.empty() && m_state != Close) {
        if (m_state == ReadingRequest) {
            size_t originalSize = bytes.size();
            if (m_request->handleData(bytes)) {
                // I have read the complete request. Digest it.
                // - Save the keepalive flag to override the Response if needed.
                m_responseKeepalive = m_request->isKeepalive();

                // - Figure out content length. RfC 2616 4.4 says Transfer-Encoding has highest precedence,
                //   then Content-Length (then multipart/byteranges which we do not support).
                afl::io::Stream::FileSize_t contentLength = 0;
                bool isChunked = false;
                if (const HeaderField* p = m_request->headers().get("Transfer-Encoding")) {
                    // possibly chunked encoding
                    if (p->getPrimaryValue() == "chunked") {
                        isChunked = true;
                    }
                } else if (const HeaderField* p = m_request->headers().get("Content-Length")) {
                    // possibly content-length
                    if (!afl::string::strToInteger(p->getPrimaryValue(), contentLength)) {
                        contentLength = 0;
                    }
                } else {
                    // keep default: no body
                }

                // Build response.
                if (m_request->hasErrors()) {
                    m_response.reset(new ErrorResponse(m_request, "400 Bad Request"));
                } else {
                    m_response.reset(m_dispatcher.createNewResponse(m_request));
                    if (m_response.get() == 0) {
                        m_response.reset(new ErrorResponse(m_request, "404 Not Found"));
                    }
                }

                // Build response sink.
                if (isChunked) {
                    m_responseSink.reset(new ChunkedSink(*m_response));
                } else {
                    m_responseSink.reset(new afl::io::LimitedDataSink(*m_response, contentLength));
                }

                m_request.reset(new Request());
                m_requestHeaderLength = 0;
                m_state = ReadingData;
            } else {
                // Request ate the complete buffer. Validate length.
                size_t remainingSize = m_maxRequestHeaderLength - m_requestHeaderLength;
                if (originalSize >= remainingSize) {
                    // This caused the size to be exceeded - fail it.
                    // FIXME: log it
                    m_state = Close;
                } else {
                    m_requestHeaderLength += originalSize;
                }
            }
        }
        if (m_state == ReadingData) {
            if (m_responseSink->handleData(bytes)) {
                // Finish response and check next state
                m_response->handleDataComplete();
                if (m_responseKeepalive && m_response->isKeepalive()) {
                    // Keep-alive allowed; proceed with next request
                    m_state = ReadingRequest;
                } else {
                    // No keep-alive; close after sending data
                    m_state = Close;
                }

                // Tell it to send
                m_finishedResponses.pushBackNew(m_response.release());
                m_responseSink.reset();
                m_responseKeepalive = false;
            }
        }
    }
}

// Handle send timeout.
void
afl::net::http::ProtocolHandler::handleSendTimeout(afl::base::ConstBytes_t /*unsentBytes*/)
{
    // FIXME: log it
    m_state = Close;
}

// Handle connection close.
void
afl::net::http::ProtocolHandler::handleConnectionClose()
{ }
