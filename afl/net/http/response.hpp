/**
  *  \file afl/net/http/response.hpp
  *  \brief Interface afl::net::http::Response
  */
#ifndef AFL_AFL_NET_HTTP_RESPONSE_HPP
#define AFL_AFL_NET_HTTP_RESPONSE_HPP

#include "afl/io/datasink.hpp"

namespace afl { namespace net { namespace http {

    /** HTTP Server Response.
        Instances of this interface serve a single request.

        The HTTP ProtocolHandler (server) calls this class' methods in the following sequence:
        - zero or more calls to handleData() to process incoming ("POST") data
        - one call to handleDataComplete() to terminate the incoming data
        - any number of getData() calls to obtain data to send until the method returns an empty descriptor.
        - at any time, isKeepalive() to check for keepalive.
        The sequence can be terminated at any time if a network problem occurs.

        The server implementation serializes calls to Response methods,
        so methods need not be synchronized against each other.
        However, methods should be synchronized against other Response's methods if they share state. */
    class Response : public afl::io::DataSink {
     public:
        /** Consume data.
            This function receives all data uploaded by the client with the request, e.g. POST data.
            Transport encoding has already been decoded.

            \param name [in] Identification of the data source, for error reporting.
            \param data [in/out] On input, data to process. On output, remaining unprocessed data.
            \retval false More data required (%data is empty)
            \retval true No more data required (%data has unprocessed data; should not be used) */
        virtual bool handleData(const String_t& name, afl::base::ConstBytes_t& data) = 0;

        /** Handle data completion.
            This function is called after the final handleData() call. */
        virtual void handleDataComplete() = 0;

        /** Check for keepalive.
            Regular requests should return true to allow keepalive if agreed upon between client and server.
            For errors, it makes sense to cancel keepalive by returning false,
            to force client and server to make a new connection and re-negotiate.
            \return keepalive flag */
        virtual bool isKeepalive() = 0;

        /** Get data to send.
            Called repeatedly by server to obtain data.
            Depending in the Request this Response was made with, should return header and/or body data.
            Return an empty ConstBytes_t to stop sending.
            \return data to send */
        virtual afl::base::ConstBytes_t getData() = 0;
    };

} } }

#endif
