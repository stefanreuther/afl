/**
  *  \file afl/net/http/request.cpp
  *  \brief Class afl::net::http::Request
  */

#include <stdio.h>
#include "afl/net/http/request.hpp"
#include "afl/net/url.hpp"

namespace {
    /** HTTP/0.9. Initial protocol version, supports only "GET /path", does not support response headers. */
    const char HTTP_0_9[] = "HTTP/0.9";

    /** HTTP/1.1. Keepalive is default, Host header is mandatory. */
    const char HTTP_1_1[] = "HTTP/1.1";

    const int Space = 1;
    const int Newline = 2;
    const int Return = 4;
    const int Other = 8;

    int scan(afl::base::ConstBytes_t& bytes, String_t* save, int accept)
    {
        while (const uint8_t* ch = bytes.at(0)) {
            int found = (*ch == ' ' ? Space
                         : *ch == '\n' ? Newline
                         : *ch == '\r' ? Return
                         : Other);
            if ((found & accept) == 0) {
                return found;
            }
            if (save != 0) {
                *save += *ch;
            }
            bytes.eat();
        }
        return 0;
    }
}

// Constructor.
afl::net::http::Request::Request()
    : m_method(),
      m_path(),
      m_version(),
      m_header(),
      m_headerParser(m_header),
      m_state(Method)
{ }

// Parse header data.
bool
afl::net::http::Request::handleData(afl::base::ConstBytes_t& bytes)
{
    while (!bytes.empty()) {
        switch (m_state) {
         case Method:
            switch (scan(bytes, &m_method, Other)) {
             case Space:   m_state = MethodSp; break;
             case Return:  m_state = MethodSp; break;
             case Newline: m_state = Error;    break;
             default:      /* keep scanning */ break;
            }
            break;
         case MethodSp:
            switch (scan(bytes, 0, Space | Return)) {
             case Newline: m_state = Error;    break;
             case Other:   m_state = Path;     break;
             default:      /* keep scanning */ break;
            }
            break;
         case Path:
            switch (scan(bytes, &m_path, Other)) {
             case Space:   m_state = PathSp;   break;
             case Return:  m_state = PathSp;   break;
             case Newline: m_version = HTTP_0_9; bytes.eat(); m_state = Done; break;
             default:      /* keep scanning */ break;
            }
            break;
         case PathSp:
            switch (scan(bytes, 0, Space | Return)) {
             case Newline: m_version = HTTP_0_9; bytes.eat(); m_state = Done; break;
             case Other:   m_state = Version;  break;
             default:      /* keep scanning */ break;
            }
            break;
         case Version:
            switch (scan(bytes, &m_version, Other | Space)) {
             case Newline: bytes.eat(); m_state = Header; break;
             case Return:  bytes.eat();        break;
             default:      /* keep scanning */ break;
            }
            break;
         case Header:
            if (m_headerParser.handleData(bytes)) {
                m_state = Done;
            }
            break;
         case Done:
         case Error:
            goto out;
        }
    }
 out:
    return (m_state == Done || m_state == Error);
}

// Get method.
const String_t&
afl::net::http::Request::getMethod() const
{
    return m_method;
}

// Get path.
const String_t&
afl::net::http::Request::getPath() const
{
    return m_path;
}

// Get version.
const String_t&
afl::net::http::Request::getVersion() const
{
    return m_version;
}

// Check for errors.
bool
afl::net::http::Request::hasErrors() const
{
    return (m_state == Error) || m_headerParser.hasErrors();
}

// Check for path match.
bool
afl::net::http::Request::matchPath(const String_t& prefix, String_t& suffix) const
{
    return afl::net::matchPath(m_path, prefix, suffix);
}

// Get headers.
const afl::net::HeaderTable&
afl::net::http::Request::headers() const
{
    return m_header;
}

// Get headers, modifiable.
afl::net::HeaderTable&
afl::net::http::Request::headers()
{
    return m_header;
}

// Check for keepalive.
bool
afl::net::http::Request::isKeepalive() const
{
    if (m_version == HTTP_0_9) {
        // HTTP/0.9 never is keepalive because it delimits messages with connection-close
        return false;
    } else if (const HeaderField* hf = m_header.get("Connection")) {
        // Explicit keep-alive request 
        return hf->getValue() == "keepalive";
    } else {
        // No header field; use default: keepalive for 1.1 only
        return m_version == HTTP_1_1;
    }
}

// Check for response header.
bool
afl::net::http::Request::isResponseHeaderRequested() const
{
    return m_version != HTTP_0_9;
}

// Check for response body.
bool
afl::net::http::Request::isResponseBodyRequested() const
{
    return m_method != "HEAD";
}
