/**
  *  \file afl/net/http/simpledownloadlistener.cpp
  *  \brief Class afl::net::http::SimpleDownloadListener
  */

#include "afl/net/http/simpledownloadlistener.hpp"
#include "afl/net/http/clientresponse.hpp"
#include "afl/net/http/client.hpp"

afl::net::http::SimpleDownloadListener::SimpleDownloadListener()
    : m_semaphore(0),
      m_sizeLimit(-1),
      m_data(),
      m_result(Succeeded),
      m_failureReason(),
      m_failureMessage()
{ }

void
afl::net::http::SimpleDownloadListener::setDataLimit(afl::io::Stream::FileSize_t limit)
{
    m_sizeLimit = limit;
}

afl::net::http::SimpleDownloadListener::Status
afl::net::http::SimpleDownloadListener::wait()
{
    m_semaphore.wait();
    return m_result;
}

afl::net::http::SimpleDownloadListener::Status
afl::net::http::SimpleDownloadListener::wait(afl::sys::Timeout_t timeout, Client& client, uint32_t requestId)
{
    if (!m_semaphore.wait(timeout)) {
        client.cancelRequest(requestId);
        m_semaphore.wait();
        if (m_result == Failed && m_failureReason == ClientRequest::Cancelled) {
            m_result = TimedOut;
        }
    }
    return m_result;
}

const afl::net::HeaderTable&
afl::net::http::SimpleDownloadListener::getResponseHeaders() const
{
    return m_headers;
}

afl::base::ConstBytes_t
afl::net::http::SimpleDownloadListener::getResponseData() const
{
    return m_data;
}

afl::net::http::ClientRequest::FailureReason
afl::net::http::SimpleDownloadListener::getFailureReason() const
{
    return m_failureReason;
}

const String_t&
afl::net::http::SimpleDownloadListener::getFailureMessage() const
{
    return m_failureMessage;
}

void
afl::net::http::SimpleDownloadListener::handleResponseHeader(ClientResponse& resp)
{
    m_headers = resp.getResponseHeaders();
}

void
afl::net::http::SimpleDownloadListener::handleResponseData(afl::io::Stream::FileSize_t ofs, afl::base::ConstBytes_t data)
{
    // FIXME: save initial offset?
    (void) ofs;
    if (data.size() > m_sizeLimit - m_data.size()) {
        data.trim(size_t(m_sizeLimit - m_data.size()));
        if (m_result == Succeeded) {
            m_result = LimitExceeded;
        }
    }
    if (!data.empty()) {
        m_data.append(data);
    }
}

void
afl::net::http::SimpleDownloadListener::handleFailure(ClientRequest::FailureReason reason, const String_t& message)
{
    m_result = Failed;
    m_failureReason = reason;
    m_failureMessage = message;
    m_semaphore.post();
}

void
afl::net::http::SimpleDownloadListener::handleSuccess()
{
    m_semaphore.post();
}

String_t
afl::net::http::toString(SimpleDownloadListener::Status st)
{
    switch (st) {
     case SimpleDownloadListener::Succeeded:     return "Succeeded";
     case SimpleDownloadListener::Failed:        return "Failed";
     case SimpleDownloadListener::TimedOut:      return "TimedOut";
     case SimpleDownloadListener::LimitExceeded: return "LimitExceeded";
    }
    return "?";
}
