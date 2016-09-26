/**
  *  \file afl/net/http/pageresponse.hpp
  *  \brief Class afl::net::http::PageResponse
  */
#ifndef AFL_AFL_NET_HTTP_PAGERESPONSE_HPP
#define AFL_AFL_NET_HTTP_PAGERESPONSE_HPP

#include "afl/net/headertable.hpp"
#include "afl/io/internalsink.hpp"

namespace afl { namespace net { namespace http {

    /** Result of a Page request.
        Objects of this class contain the result of a Page::handleRequest() call.

        The object is configured by the Page.
        The PageDispatcher can then use finish() to finish it up, and send the result to the client. */
    class PageResponse {
     public:
        // FIXME: these values don't belong here.
        enum {
            // 1xx: informational, 2xx: success, 3xx: redirect, 4xx: client error, 5xx: server error
            OK = 200,
            REDIRECT_FOUND = 302,
            BAD_REQUEST = 400,
            NOT_FOUND = 404,
            METHOD_NOT_ALLOWED = 405,
            INTERNAL_SERVER_ERROR = 500
        };

        /** Constructor.
            Makes a successful default response. */
        PageResponse();

        /** Set status code.
            \param statusCode HTTP status code (200=OK, 404=Not found, etc.) */
        void setStatusCode(int statusCode);

        /** Get status code.
            \return HTTP status code */
        int getStatusCode() const;

        /** Get status text.
            \return descriptive text for the current status code. */
        String_t getStatusText() const;

        /** Access response headers.
            \return response headers */
        const HeaderTable& headers() const;

        /** Access response headers, mutable version.
            Use this to add response headers, in particular, Content-Type.
            \return response headers */
        HeaderTable& headers();

        /** Access response message body.
            \return response message body */
        const afl::io::InternalSink& body() const;

        /** Access response message body, mutable version.
            Use this to produce the response message.
            \return response message body */
        afl::io::InternalSink& body();

        /** Shortcut: make redirection response.
            Configures this object to produce a redirect response.
            No other function calls on the object are needed, except for possibly some additional headers.

            FIXME: the target address is required to be absolute by RFC 2616. What help can we give here?

            \param address target address */
        void setRedirect(const String_t& address);

        /** Finish the request.
            Performs some routine cleanups.
            - if the result is an error, but no content has been provided, generates a simple error document
            - add a Content-Length header to support keepalive */
        void finish();

     private:
        int m_statusCode;
        HeaderTable m_headers;
        afl::io::InternalSink m_body;
    };

} } }

#endif
