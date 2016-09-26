/**
  *  \file afl/net/http/errorresponse.hpp
  *  \brief Class afl::net::http::ErrorResponse
  */
#ifndef AFL_AFL_NET_HTTP_ERRORRESPONSE_HPP
#define AFL_AFL_NET_HTTP_ERRORRESPONSE_HPP

#include <memory>
#include "afl/string/string.hpp"
#include "afl/net/http/response.hpp"

namespace afl { namespace net { namespace http {

    class Request;

    /** HTTP Server Error Response.
        Generates a simple HTTP error message.
        This class can be used for all sorts of failing HTTP requests. */
    class ErrorResponse : public Response {
     public:
        /** Constructor.
            \param req [in] original request
            \param text [in] error message text ("404 Not found") */
        ErrorResponse(const std::auto_ptr<Request>& req, String_t text);

        // Response:
        virtual bool handleData(const String_t& name, afl::base::ConstBytes_t& data);
        virtual void handleDataComplete();
        virtual bool isKeepalive();
        virtual afl::base::ConstBytes_t getData();

     private:
        String_t m_message;
        bool m_sent;
    };

} } }

#endif
