/**
  *  \file afl/net/http/errorresponse.cpp
  *  \brief Class afl::net::http::ErrorResponse
  */

#include "afl/net/http/errorresponse.hpp"
#include "afl/string/format.hpp"
#include "afl/net/http/request.hpp"

// Constructor.
afl::net::http::ErrorResponse::ErrorResponse(const std::auto_ptr<Request>& req, String_t text)
    : m_message(),
      m_sent(false)
{
    if (req->isResponseHeaderRequested()) {
        m_message += afl::string::Format("%s %s\r\n"
                                         "Connection: close\r\n"
                                         "Content-Type: text/plain; charset=UTF-8\r\n"
                                         "Content-Length: %d\r\n\r\n",
                                         req->getVersion(), text, text.size());
    }
    if (req->isResponseBodyRequested()) {
        m_message += text;
    }
}

// Ignore all incoming data.
bool
afl::net::http::ErrorResponse::handleData(const String_t& /*name*/, afl::base::ConstBytes_t& /*data*/)
{
    return false;
}

// Ignore all incoming data.
void
afl::net::http::ErrorResponse::handleDataComplete()
{ }

// For safety, ErrorResponse cancel keepalive to force the browser to make a new connection.
bool
afl::net::http::ErrorResponse::isKeepalive()
{
    return false;
}

// Get data to send.
afl::base::ConstBytes_t
afl::net::http::ErrorResponse::getData()
{
    afl::base::ConstBytes_t result;
    if (!m_sent) {
        m_sent = true;
        result = afl::string::toBytes(m_message);
    }
    return result;
}
