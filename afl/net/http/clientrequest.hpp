/**
  *  \file afl/net/http/clientrequest.hpp
  *  \brief Interface afl::net::http::ClientRequest
  */
#ifndef AFL_AFL_NET_HTTP_CLIENTREQUEST_HPP
#define AFL_AFL_NET_HTTP_CLIENTREQUEST_HPP

#include "afl/base/deletable.hpp"
#include "afl/base/memory.hpp"
#include "afl/net/name.hpp"
#include "afl/string/string.hpp"

namespace afl { namespace net { namespace http {

    class ClientResponse;

    /** HTTP Client Request Base Class.
        Actual requests derive from this class.

        Function calls fall in three categories:
        - inquiry (getName(), getScheme(), isHeadRequest()):
          these can be called at any time and tell about this request
        - request processing:
          - getRequestData() is called repeatedly to submit the request
          - restart() is called if there is a problem submitting the request
          - handleResponseHeader(), handleResponseData() are called when we have a response
        - finalisation: exactly one of handleFailure() or handleSuccess() is called before
          the ClientRequest is destroyed.

        As a general rule, ClientRequest objects are owned by the http::Client they're submitted to,
        and may not be accessed by anyone else.

        Because the callbacks run in the HTTP client's thread, they should not block. */
    class ClientRequest : public afl::base::Deletable {
     public:
        /** Error code for handleFailure(). */
        enum FailureReason {
            Cancelled,             // The request has been explicitly cancelled.
            ConnectionFailed,      // Could not connect to the target (host not found).
            ConnectionClosed,      // The server closed the connection although the transfer was not yet complete.
            UnsupportedProtocol,   // We do not speak the requested protocol.
            NetworkError,          // Other network error.
            ServerError            // Server error, i.e. we don't understand what it's saying.
        };

        /** Constructor. */
        ClientRequest();

        /** Inquiry: get name (=host+port) for this request.
            Can be called at any time.
            \return name */
        virtual Name getName() const = 0;

        /** Inquiry: get scheme (=protocol) for this request.
            Can be called at any time.
            \return scheme ("http") */
        virtual String_t getScheme() const = 0;

        /** Inquiry: check for HEAD request.
            Can be called at any time.
            \retval true this is a HEAD request and we do not expect a message body
            \retval false this is not a HEAD request, we possibly expect a message body */
        virtual bool isHeadRequest() const = 0;

        /** Request processing: get data to send.
            Should return data to send to the server.
            This function is called repeatedly to get new data when the previous data was transmitted.
            If the request has been completely sent, this function should return a null descriptor
            as a signal to start receiving the response.
            \return data to send */
        virtual afl::base::ConstBytes_t getRequestData() = 0;

        /** Request processing: restart.
            This function is called by the http::Client if there was a problem with submitting the request.
            It should reset the internal state (in particular, the next getRequestData()
            should return the first part of the request again).
            The ClientRequest should limit the number of restarts.
            \retval true restart allowed
            \retval false restart not allowed, fail the request instead */
        virtual bool restart() = 0;

        /** Request processing: response header completed.
            This function is called by the http::Client when the response headers have been received.
            \param resp Response headers */
        virtual void handleResponseHeader(ClientResponse& resp) = 0;

        /** Request processing: response data.
            This function is called by the http::Client for each chunk of data received from the server.
            \param data Descriptor for the data */
        virtual void handleResponseData(afl::base::ConstBytes_t data) = 0;

        /** Request finalisation: failure.
            This function is called by the http::Client if the request cannot be successfully completed.
            \param reason Failure reason
            \param message Additional message */
        virtual void handleFailure(FailureReason reason, const String_t& message) = 0;

        /** Request finalisation: success.
            This function is called by the http::Client if the request completed successfully. */
        virtual void handleSuccess() = 0;


        /** Set request identifier.
            This is used by the Client to identify the request.
            \param id request identifier */
        void setRequestId(uint32_t id);

        /** Get request identifier.
            \return request identifier */
        uint32_t getRequestId() const;

     private:
        uint32_t m_requestId;
    };

} } }

inline
afl::net::http::ClientRequest::ClientRequest()
    : m_requestId(0)
{ }

inline void
afl::net::http::ClientRequest::setRequestId(uint32_t id)
{
    m_requestId = id;
}

inline uint32_t
afl::net::http::ClientRequest::getRequestId() const
{
    return m_requestId;
}

#endif
