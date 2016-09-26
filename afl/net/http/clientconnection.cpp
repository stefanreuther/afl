/**
  *  \file afl/net/http/clientconnection.cpp
  *  \brief Class afl::net::http::ClientConnection
  */

#include <cassert>
#include "afl/net/http/clientconnection.hpp"
#include "afl/net/http/clientrequest.hpp"
#include "afl/net/http/clientresponse.hpp"
#include "afl/net/http/chunkedsink.hpp"
#include "afl/io/limiteddatasink.hpp"
#include "afl/io/inflatedatasink.hpp"
#include "afl/string/messages.hpp"

namespace {
    /** Timeout for keeping an idle connection, in milliseconds.
        If an idle keepalive connection does not get a new request within this time, the connection is closed. */
    const uint32_t IDLE_TIMEOUT = 30000;

    /** Timeout for network I/O, in milliseconds.
        If a requested network I/O does not proceed within this time, the connection fails. */
    const uint32_t NETWORK_TIMEOUT = 30000;


    class ResponseSink : public afl::io::DataSink {
     public:
        ResponseSink(afl::net::http::ClientRequest* req)
            : m_request(req)
            { }
        virtual bool handleData(const String_t& /*name*/, afl::base::ConstBytes_t& data)
            {
                m_request->handleResponseData(data);
                data.reset();
                return false;
            }
     private:
        afl::net::http::ClientRequest* m_request;
    };
}


afl::net::http::ClientConnection::ClientConnection(const Name& name, const String_t& scheme, afl::base::Ptr<Socket> socket)
    : m_name(name),
      m_scheme(scheme),
      m_socket(socket),
      m_request(),
      m_response(),
      m_responseDeleter(),
      m_pResponseSink(0),
      m_state(WantWait),
      m_idleTime(0),
      m_sendOperation(),
      m_receiveOperation()
{ }

afl::net::http::ClientConnection::~ClientConnection()
{
    // If we're destroyed while we still have a request, cancel that.
    if (m_request.get() != 0) {
        m_request->handleFailure(ClientRequest::Cancelled, afl::string::Messages::operationCancelled());
        reset();
    }
}

afl::net::http::ClientConnection::Action
afl::net::http::ClientConnection::handleEvent(afl::async::Controller& ctl, afl::async::Operation* p, uint32_t elapsed)
{
    // Advance time
    m_idleTime += elapsed;

    // Process input
    if (m_state == DuringSend && p == &m_sendOperation) {
        // Send operation finished. Check whether it actually completed,
        // otherwise retrigger the missing part.
        if (m_sendOperation.isCompleted()) {
            // ok, complete
            m_state = BeforeSend;
        } else if (m_sendOperation.getNumSentBytes() == 0) {
            // no progress
            if (!m_request->restart()) {
                m_request->handleFailure(ClientRequest::NetworkError, afl::string::Messages::networkConnectionLost());
                reset();
            }
            m_state = WantClose;
        } else {
            // progress
            m_sendOperation.setData(m_sendOperation.getUnsentBytes());
            m_socket->sendAsync(ctl, m_sendOperation);
            m_state = DuringSend;
            m_idleTime = 0;
        }
    }

    if (m_state == DuringReceiveHeader && p == &m_receiveOperation) {
        // Receive header operation completed.
        if (m_receiveOperation.getNumReceivedBytes() == 0) {
            // Connection close.
            // If we can restart the request, leave it there; main loop will give it to our next instance.
            // Otherwise, just finish it.
            if (!m_request->restart()) {
                m_request->handleFailure(ClientRequest::ServerError, afl::string::Messages::networkConnectionLost());
                reset();
            }
            m_state = WantClose;
        } else {
            // Success
            handleReception(m_receiveOperation.getReceivedBytes());
        }
    }

    if (m_state == DuringReceivePayload && p == &m_receiveOperation) {
        // Receive payload operation completed.
        if (m_receiveOperation.getNumReceivedBytes() == 0) {
            // Connection close.
            // For StreamLimit, this is a success, otherwise it's a failure.
            if (m_response->getResponseLimitKind() == ClientResponse::StreamLimit) {
                m_request->handleSuccess();
            } else {
                m_request->handleFailure(ClientRequest::ConnectionClosed, afl::string::Messages::networkConnectionLost());
            }
            reset();
            m_state = WantClose;
        } else {
            handleReception(m_receiveOperation.getReceivedBytes());
        }
    }

    if (m_state == BeforeSend) {
        // We want to send. Check what we have. If we don't have anything, proceed to receiving.
        m_sendOperation.setData(m_request->getRequestData());
        if (m_sendOperation.isCompleted()) {
            m_state = BeforeReceiveHeader;
        } else {
            m_socket->sendAsync(ctl, m_sendOperation);
            m_state = DuringSend;
            m_idleTime = 0;
        }
    }

    if (m_state == BeforeReceiveHeader) {
        // We want to receive headers.
        m_receiveOperation.setData(m_receiveBuffer);
        m_socket->receiveAsync(ctl, m_receiveOperation);
        m_state = DuringReceiveHeader;
        m_idleTime = 0;
    }

    if (m_state == BeforeReceivePayload) {
        // We want to receive payload.
        m_receiveOperation.setData(m_receiveBuffer);
        m_socket->receiveAsync(ctl, m_receiveOperation);
        m_state = DuringReceivePayload;
        m_idleTime = 0;
    }

    // Check timeout
    switch (m_state) {
     case WantWait:
        if (m_idleTime > IDLE_TIMEOUT) {
            m_state = WantClose;
        }
        break;

     case DuringSend:
        // Request send timeout. This is a restartable error.
        if (m_idleTime > NETWORK_TIMEOUT) {
            m_socket->cancel(ctl, m_sendOperation);
            if (!m_request->restart()) {
                m_request->handleFailure(ClientRequest::NetworkError, afl::string::Messages::networkConnectionLost());
                reset();
            }
            m_state = WantClose;
        }
        break;

     case DuringReceiveHeader:
        // Header reception timeout. This is a restartable error.
        if (m_idleTime > NETWORK_TIMEOUT) {
            m_socket->cancel(ctl, m_receiveOperation);
            if (!m_request->restart()) {
                m_request->handleFailure(ClientRequest::NetworkError, afl::string::Messages::networkConnectionLost());
                reset();
            }
            m_state = WantClose;
        }
        break;

     case DuringReceivePayload:
        // Payload reception timeout. This is a non-restartable error.
        if (m_idleTime > NETWORK_TIMEOUT) {
            m_socket->cancel(ctl, m_receiveOperation);
            m_request->handleFailure(ClientRequest::NetworkError, afl::string::Messages::networkConnectionLost());
            reset();
            m_state = WantClose;
        }
        break;

     case BeforeReceiveHeader:
     case BeforeReceivePayload:
     case BeforeSend:
     case WantClose:
        // No timeout in these states
        break;
    }

    // Generate return value
    return (m_state == WantClose ? Shutdown
            : m_state == WantWait ? WaitForRequest
            : Transferring);
}

void
afl::net::http::ClientConnection::setNewRequest(ClientRequest* req)
{
    assert(m_state == WantWait);
    assert(req != 0);
    m_request.reset(req);
    m_response.reset(new ClientResponse(req->isHeadRequest()));
    m_responseDeleter.clear();
    m_pResponseSink = 0;
    m_state = BeforeSend;
}

afl::net::http::ClientRequest*
afl::net::http::ClientConnection::extractRequest()
{
    assert(m_state == WantClose);
    m_response.reset();
    m_responseDeleter.clear();
    m_pResponseSink = 0;
    return m_request.release();
}

bool
afl::net::http::ClientConnection::matchRequest(const ClientRequest& req) const
{
    return req.getName() == m_name
        && req.getScheme() == m_scheme;
}

void
afl::net::http::ClientConnection::cancelRequest(afl::async::Controller& ctl, uint32_t id)
{
    if (m_request.get() != 0 && m_request->getRequestId() == id) {
        cancel(ctl);
    }
}

void
afl::net::http::ClientConnection::cancel(afl::async::Controller& ctl)
{
    if (m_request.get() != 0) {
        // OK, kill it
        switch (m_state) {
         case WantWait:             // cannot happen
         case BeforeSend:           // cannot happen
         case BeforeReceiveHeader:  // cannot happen
         case BeforeReceivePayload: // cannot happen
         case WantClose:            // cannot happen
            break;

         case DuringSend:           // send active
            m_socket->cancel(ctl, m_sendOperation);
            break;

         case DuringReceiveHeader:  // receive active
         case DuringReceivePayload: // receive active
            m_socket->cancel(ctl, m_receiveOperation);
            break;
        }

        // Finish and forget the request
        m_request->handleFailure(ClientRequest::Cancelled, afl::string::Messages::operationCancelled());
        reset();

        // We have now violated protocol. Close the socket.
        m_state = WantClose;
    }
}

void
afl::net::http::ClientConnection::handleReception(afl::base::ConstBytes_t data)
{
    try {
        if (m_state == DuringReceiveHeader) {
            // I was receiving headers. Check whether we finish that.
            if (m_response->handleData(m_name.toString(), data)) {
                // Finished the header.
                m_request->handleResponseHeader(*m_response);

                // Make a sink to process it.
                // (Note that this will call m_responseSink with 0 data;
                // this is required for proper state transitions with 0-byte body.)
                m_pResponseSink = &m_responseDeleter.addNew(new ResponseSink(m_request.get()));

                // Content-Encoding:
                switch (m_response->getResponseEncoding()) {
                 case ClientResponse::IdentityEncoding:
                    break;

                 case ClientResponse::GzipEncoding:
                    m_pResponseSink = &m_responseDeleter.addNew(new afl::io::InflateDataSink(*m_pResponseSink, afl::io::InflateDataSink::Gzip));
                    break;

                 case ClientResponse::DeflateEncoding:
                    m_pResponseSink = &m_responseDeleter.addNew(new afl::io::InflateDataSink(*m_pResponseSink, afl::io::InflateDataSink::Raw));
                    break;

                 case ClientResponse::UnknownEncoding:
                    // FIXME: handle as error
                    break;
                }

                // Transfer-Encoding:
                switch (m_response->getResponseLimitKind()) {
                 case ClientResponse::ByteLimit:
                    // limit number of bytes to receive
                    m_pResponseSink = &m_responseDeleter.addNew(new afl::io::LimitedDataSink(*m_pResponseSink, m_response->getResponseLength()));
                    break;

                 case ClientResponse::ChunkLimit:
                    m_pResponseSink = &m_responseDeleter.addNew(new ChunkedSink(*m_pResponseSink));
                    break;

                 case ClientResponse::StreamLimit:
                    // no additional sink
                    break;
                }
                m_state = DuringReceivePayload;
            } else {
                // Need more header
                m_state = BeforeReceiveHeader;
            }
        }

        if (m_state == DuringReceivePayload) {
            // I was receiving payloads. Check whether we finish that.
            // FIXME: exception handling!
            if (m_pResponseSink->handleData(m_name.toString(), data)) {
                // Finished the payload -> finish off the request
                m_request->handleSuccess();
                reset();
                m_state = WantWait;
            } else {
                // Need more payload
                m_state = BeforeReceivePayload;
            }
        }
    }
    catch (std::exception& e) {
        m_request->handleFailure(ClientRequest::ServerError, e.what());
        reset();
        m_state = WantClose;
    }
    catch (...) {
        m_request->handleFailure(ClientRequest::ServerError, afl::string::Messages::unknownError());
        reset();
        m_state = WantClose;
    }
}

void
afl::net::http::ClientConnection::reset()
{
    m_responseDeleter.clear();
    m_pResponseSink = 0;
    m_response.reset();
    m_request.reset();
}
