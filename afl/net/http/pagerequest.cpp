/**
  *  \file afl/net/http/pagerequest.cpp
  *  \brief Class afl::net::http::PageRequest
  */

#include "afl/net/http/pagerequest.hpp"
#include "afl/net/url.hpp"
#include "afl/string/string.hpp"

afl::net::http::PageRequest::PageRequest(String_t rootPath, String_t selfPath, String_t path)
    : m_headers(),
      m_body(),
      m_arguments(),
      m_method("GET"),
      m_rootPath(rootPath),
      m_selfPath(selfPath),
      m_path(path),
      m_inputStatus(Undecided),
      m_formParser(m_arguments)
{
    matchArguments(m_path, m_arguments);
}

void
afl::net::http::PageRequest::setIgnoreData()
{
    m_inputStatus = Ignore;
}

void
afl::net::http::PageRequest::handleData(afl::base::ConstBytes_t data)
{
    if (m_inputStatus == Undecided) {
        // Determine what to do with a potential body
        if (m_method == "GET" || m_method == "HEAD") {
            // These never have a body, so don't even try
            m_inputStatus = Ignore;
        } else if (HeaderField* hf = m_headers.get("Content-Type")) {
            // Might be form data.
            String_t type = hf->getPrimaryValue();
            if (afl::string::strCaseCompare(type, "application/x-www-form-urlencoded") == 0) {
                // Parse as form
                m_inputStatus = ParseForm;
            } else if (afl::string::strCaseCompare(type, "multipart/form-data") == 0) {
                // FIXME: parse as multipart
                m_inputStatus = Ignore;
            } else {
                // Store as upload
                m_inputStatus = SaveBody;
            }
        } else {
            // No interpretable body
            m_inputStatus = Ignore;
        }
    }
    switch (m_inputStatus) {
     case Undecided:
     case Ignore:
        break;

     case SaveBody:
        m_body.handleData(data);
        break;

     case ParseForm:
        m_formParser.handleData(data);
        break;
    }
}

void
afl::net::http::PageRequest::finish()
{
    switch (m_inputStatus) {
     case Undecided:
     case Ignore:
     case SaveBody:
        break;

     case ParseForm:
        m_formParser.handleDataComplete();
        break;
    }
}

const afl::net::HeaderTable&
afl::net::http::PageRequest::headers() const
{
    return m_headers;
}

afl::net::HeaderTable&
afl::net::http::PageRequest::headers()
{
    return m_headers;
}

const afl::io::InternalSink&
afl::net::http::PageRequest::body() const
{
    return m_body;
}

afl::io::InternalSink&
afl::net::http::PageRequest::body()
{
    return m_body;
}

const afl::net::HeaderTable&
afl::net::http::PageRequest::arguments() const
{
    return m_arguments;
}
afl::net::HeaderTable&
afl::net::http::PageRequest::arguments()
{
    return m_arguments;
}

void
afl::net::http::PageRequest::setMethod(const String_t& method)
{
    m_method = method;
}

const String_t&
afl::net::http::PageRequest::getMethod() const
{
    return m_method;
}

void
afl::net::http::PageRequest::setRootPath(const String_t& rootPath)
{
    m_rootPath = rootPath;
}

const String_t&
afl::net::http::PageRequest::getRootPath() const
{
    return m_rootPath;
}

void
afl::net::http::PageRequest::setSelfPath(const String_t& selfPath)
{
    m_selfPath = selfPath;
}

const String_t&
afl::net::http::PageRequest::getSelfPath() const
{
    return m_selfPath;
}

void
afl::net::http::PageRequest::setPath(const String_t& path)
{
    m_path = path;
}

const String_t&
afl::net::http::PageRequest::getPath() const
{
    return m_path;
}
