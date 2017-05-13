/**
  *  \file afl/net/http/protocolhandler.hpp
  *  \class afl::net::http::ProtocolHandler
  */
#ifndef AFL_AFL_NET_HTTP_PROTOCOLHANDLER_HPP
#define AFL_AFL_NET_HTTP_PROTOCOLHANDLER_HPP

#include <memory>
#include "afl/container/ptrqueue.hpp"
#include "afl/io/datasink.hpp"
#include "afl/net/protocolhandler.hpp"

namespace afl { namespace net { namespace http {

    class Dispatcher;
    class Request;
    class Response;

    /** HTTP Server Protocol Handler.
        This implements a single HTTP connection that can serve multiple requests.
        It parses the HTTP protocol and uses the specified Dispatcher to route requests.

        Supported features:
        - HTTP/0.9, HTTP/1.0 and HTTP/1.1
        - persistent connections
        - request bodies (for POST etc.) using Content-Length or Transfer-Encoding delimiting
        - limited pipelining */
    class ProtocolHandler : public afl::net::ProtocolHandler {
     public:
        /** Constructor.
            \param dispatcher Dispatcher to use for request routing */
        explicit ProtocolHandler(Dispatcher& dispatcher);

        /** Destructor. */
        virtual ~ProtocolHandler();

        // ProtocolHandler:
        virtual void getOperation(Operation& op);
        virtual void advanceTime(afl::sys::Timeout_t msecs);
        virtual void handleData(afl::base::ConstBytes_t bytes);
        virtual void handleSendTimeout(afl::base::ConstBytes_t unsentBytes);
        virtual void handleConnectionClose();

     private:
        /** Dispatcher. */
        Dispatcher& m_dispatcher;

        /** Input state.
            This class consists of two almost-independant state machines.
            Input state is determined by this value.
            It produces Request's and feeds the Response's created by that.
            Once a Response has finished receiving, it is moved to m_finishedResponses which drives output state (=sending). */
        enum State {
            ReadingRequest,     // Read a request
            ReadingData,        // Read data for a request
            Close               // Send data, then close
        };
        State m_state;

        /** Currently-receiving request. Never null. */
        std::auto_ptr<Request> m_request;

        /** Currently-receiving response.
            We have determined where to route the request, and this is the target.
            Actual data goes into m_responseSink. */
        std::auto_ptr<Response> m_response;

        /** Currently-receiving response, data sink.
            This parses the request body delimiters and drives m_response. */
        std::auto_ptr<afl::io::DataSink> m_responseSink;

        /** Currently-receiving response, keepalive flag. */
        bool m_responseKeepalive;

        /** All outgoing responses. */
        afl::container::PtrQueue<Response> m_finishedResponses;
    };

} } }

#endif
