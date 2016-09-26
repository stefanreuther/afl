/**
  *  \file afl/net/http/pageresponse.cpp
  *  \brief Class afl::net::http::PageResponse
  */

#include "afl/net/http/pageresponse.hpp"
#include "afl/string/format.hpp"

afl::net::http::PageResponse::PageResponse()
    : m_statusCode(OK)
{ }

void
afl::net::http::PageResponse::setStatusCode(int statusCode)
{
    m_statusCode = statusCode;
}

int
afl::net::http::PageResponse::getStatusCode() const
{
    return m_statusCode;
}

String_t
afl::net::http::PageResponse::getStatusText() const
{
    const char* result;
    switch (m_statusCode) {
     case PageResponse::OK:                    result = "OK";                    break;
     case PageResponse::REDIRECT_FOUND:        result = "Found";                 break;
     case PageResponse::BAD_REQUEST:           result = "Bad Request";           break;
     case PageResponse::NOT_FOUND:             result = "Not Found";             break;
     case PageResponse::METHOD_NOT_ALLOWED:    result = "Method Not Allowed";    break;
     case PageResponse::INTERNAL_SERVER_ERROR: result = "Internal Server Error"; break;
     default:
        switch (m_statusCode / 100) {
         case 1:  result = "Informative";  break;
         case 2:  result = "Success";      break;
         case 3:  result = "Redirect";     break;
         case 4:  result = "Client Error"; break;
         case 6:  result = "Server Error"; break;
         default: result = "Error";        break;
        }
    }
    return result;
}

const afl::net::HeaderTable&
afl::net::http::PageResponse::headers() const
{
    return m_headers;
}

afl::net::HeaderTable&
afl::net::http::PageResponse::headers()
{
    return m_headers;
}

const afl::io::InternalSink&
afl::net::http::PageResponse::body() const
{
    return m_body;
}

afl::io::InternalSink&
afl::net::http::PageResponse::body()
{
    return m_body;
}

void
afl::net::http::PageResponse::setRedirect(const String_t& address)
{
    setStatusCode(REDIRECT_FOUND);
    headers().set("Location", address);
}

void
afl::net::http::PageResponse::finish()
{
    // We have an error code and no content. Add some.
    if (m_body.getContent().empty() && m_statusCode >= 300) {
        String_t errorDocument = getStatusText();
        afl::base::ConstBytes_t bytes(afl::string::toBytes(errorDocument));
        m_body.handleData("<PageResponse>", bytes);
        m_headers.add("Content-Type", "text/plain; charset=UTF-8");
    }

    // Add correct content length
    // FIXME: deal with byte serving / partial requests here!
    m_headers.set("Content-Length", afl::string::Format("%d", m_body.getContent().size()));
}
