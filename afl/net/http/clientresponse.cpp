/**
  *  \file afl/net/http/clientresponse.cpp
  *  \brief Class afl::net::http::ClientResponse
  */

#include "afl/net/http/clientresponse.hpp"
#include "afl/string/parse.hpp"
#include "afl/string/char.hpp"
#include "afl/string/parsememory.hpp"

namespace {
    const size_t MAX_PHRASE  = 200;
    const size_t MAX_VERSION = 32;
}

afl::net::http::ClientResponse::ClientResponse(bool isHeadRequest)
    : afl::io::DataSink(),
      HeaderConsumer(),
      m_state(ParseVersion),
      m_version(),
      m_statusCode(0),
      m_statusPhrase(),
      m_isHeadRequest(isHeadRequest),
      m_hasContentLength(false),
      m_contentLength(0),
      m_hasContentRange(false),
      m_contentRangeStart(0),
      m_contentRangeLength(0),
      m_contentRangeTotal(0),
      m_hasConnection(false),
      m_isConnectionKeepalive(false),
      m_hasChunkedTransferEncoding(false),
      m_contentEncoding(IdentityEncoding),
      m_headers(),
      m_cookies(),
      m_headerParser(*this)
{ }

afl::net::http::ClientResponse::~ClientResponse()
{ }

bool
afl::net::http::ClientResponse::handleData(afl::base::ConstBytes_t& data)
{
    // Parse version
    if (m_state == ParseVersion) {
        while (const uint8_t* pch = data.eat()) {
            if (*pch == ' ' || *pch == '\t') {
                // next field
                m_state = ParseStatusCode;
                break;
            } else if (*pch >= ' ' && *pch < 0x7F && m_version.size() < MAX_VERSION) {
                // ok
                m_version += char(*pch);
            } else {
                // syntax error
                // (overlong version also is a syntax error)
                m_state      = FinalError;
                m_statusCode = 500;
                break;
            }
        }
    }

    // Parse status code
    if (m_state == ParseStatusCode) {
        while (const uint8_t* pch = data.eat()) {
            if (*pch == ' ' || *pch == '\t') {
                // next field
                m_state = ParseStatusPhrase;
                break;
            } else if (*pch >= '0' && *pch <= '9') {
                // ok
                m_statusCode = 10*m_statusCode + (*pch - '0');
            } else {
                // syntax error
                m_state      = FinalError;
                m_statusCode = 500;
                break;
            }
        }
    }

    // Parse phrase
    if (m_state == ParseStatusPhrase) {
        while (const uint8_t* pch = data.eat()) {
            if (*pch == '\r') {
                // ignore
            } else if (*pch == '\n') {
                // ok
                m_state = ParseHeader;
                break;
            } else {
                // success (overlong phrase is not a syntax error)
                if (m_statusPhrase.size() < MAX_PHRASE) {
                    m_statusPhrase += char(*pch);
                }
            }
        }
    }

    // Parse header
    if (m_state == ParseHeader) {
        if (m_headerParser.handleData(data)) {
            m_state = FinalSuccess;
        }
    }

    // Result
    return m_state == FinalSuccess || m_state == FinalError;
}

void
afl::net::http::ClientResponse::handleHeader(String_t key, String_t value)
{
    if (afl::string::strCaseCompare(key, "Content-Length") == 0) {
        // Content-Length: gives a fixed size of content
        FileSize_t n;
        if (afl::string::strToInteger(value, n)) {
            m_hasContentLength = true;
            m_contentLength = n;
        }
    } else if (afl::string::strCaseCompare(key, "Content-Range") == 0) {
        // Content-Range: describes if we only get a partial range
        FileSize_t beg;
        FileSize_t end;
        FileSize_t tot;
        afl::string::ConstStringMemory_t mem(afl::string::toMemory(value));
        size_t z = 0;  // hack to silence warning
        if (   afl::string::parseIf     (mem, afl::string::charIsSpace) >= z
            && afl::string::parseLiteral(mem, "bytes")                   > 0
            && afl::string::parseIf     (mem, afl::string::charIsSpace)  > 0
            && afl::string::parseNumber (mem, beg)                       > 0
            && afl::string::parseChar   (mem, '-')                       > 0
            && afl::string::parseNumber (mem, end)                       > 0
            && afl::string::parseChar   (mem, '/')                       > 0
            && afl::string::parseNumber (mem, tot)                       > 0
            && afl::string::parseIf     (mem, afl::string::charIsSpace) >= z
            && mem.empty()
            && beg <= end+1
            && beg <= tot
            && end < tot)
        {
            m_hasContentRange   = true;
            m_contentRangeStart  = beg;
            m_contentRangeLength = end - beg + 1;
            m_contentRangeTotal  = tot;
        }
    } else if (afl::string::strCaseCompare(key, "Connection") == 0) {
        // Connection: close/keep-alive to override the default implied by the request
        if (afl::string::strCaseCompare(value, "close") == 0) {
            m_hasConnection = true;
            m_isConnectionKeepalive = false;
        } else if (afl::string::strCaseCompare(value, "keep-alive") == 0) {
            m_hasConnection = true;
            m_isConnectionKeepalive = true;
        } else {
            // bad value
        }
    } else if (afl::string::strCaseCompare(key, "Transfer-Encoding") == 0) {
        // Transfer-Encoding:
        if (afl::string::strCaseCompare(value, "chunked") == 0) {
            m_hasChunkedTransferEncoding = true;
        }
    } else if (afl::string::strCaseCompare(key, "Content-Encoding") == 0) {
        // Content-Encoding:
        if (afl::string::strCaseCompare(value, "gzip") == 0) {
            m_contentEncoding = GzipEncoding;
        } else if (afl::string::strCaseCompare(value, "deflate") == 0) {
            m_contentEncoding = DeflateEncoding;
        } else if (afl::string::strCaseCompare(value, "identity") == 0) {
            m_contentEncoding = IdentityEncoding;
        } else {
            m_contentEncoding = UnknownEncoding;
        }
    } else if (afl::string::strCaseCompare(key, "Set-Cookie") == 0) {
        // Cookie
        String_t::size_type eq = value.find_first_of(";=");
        if (eq != String_t::npos && value[eq] == '=') {
            m_cookies.add(afl::string::strTrim(value.substr(0, eq)),
                          afl::string::strTrim(value.substr(eq+1)));
        } else {
            // bad value
        }
    } else {
        // other
        m_headers.add(key, value);
    }
}

bool
afl::net::http::ClientResponse::hasErrors() const
{
    return m_state == FinalError
        || m_headerParser.hasErrors()
        || m_statusCode < 100
        || m_statusCode >= 1000;
}

bool
afl::net::http::ClientResponse::hasBody() const
{
    return !(m_isHeadRequest
             || m_statusCode < 200
             || m_statusCode == 204 /* no content */
             || m_statusCode == 304 /* not modified */);
}


/*
 *  Response Limits
 *
 *  Order of questions in the following function always is the same:
 *  - if we know it has no body, it's a 0-byte byte limit
 *  - if it says chunked encoding, that's the limit (length is informative in this case)
 *  - if it has a content range, that's the limit
 *  - if it has a content length, that's the limit
 *  - otherwise, we don't know how long the limit is
 */

afl::net::http::ClientResponse::Limit
afl::net::http::ClientResponse::getResponseLimitKind() const
{
    return !hasBody()
        ? ByteLimit
        : m_hasChunkedTransferEncoding
        ? ChunkLimit
        : m_hasContentRange
        ? ByteLimit
        : m_hasContentLength
        ? ByteLimit
        : StreamLimit;
}

afl::net::http::ClientResponse::Encoding
afl::net::http::ClientResponse::getResponseEncoding() const
{
    return m_contentEncoding;
}

afl::net::http::ClientResponse::FileSize_t
afl::net::http::ClientResponse::getResponseLength() const
{
    return !hasBody()
        ? 0
        : m_hasContentRange
        ? m_contentRangeLength
        : m_hasContentLength
        ? m_contentLength
        : 0;
}

afl::net::http::ClientResponse::FileSize_t
afl::net::http::ClientResponse::getResponseOffset() const
{
    return !hasBody()
        ? 0
        : m_hasContentRange
        ? m_contentRangeStart
        : 0;
}

afl::net::http::ClientResponse::FileSize_t
afl::net::http::ClientResponse::getTotalLength() const
{
    return m_hasContentRange
        ? m_contentRangeTotal
        : m_hasContentLength
        ? m_contentLength
        : 0;
}

/*
 *  Keepalive handling: determine solely based upon the server's reported
 *  HTTP version; if the client wants to successfully deviate from that, the server
 *  will have confirmed that with a Connection header.
 *
 *  However, if we do not have a means of delimiting the response,
 *  we cannot do keepalive.
 *  The same goes for errors when we don't know whether the server understood us.
 */

bool
afl::net::http::ClientResponse::isKeepalive() const
{
    return getResponseLimitKind() == StreamLimit || hasErrors()
        ? false
        : m_hasConnection
        ? m_isConnectionKeepalive
        : m_version == "HTTP/1.1";
}
