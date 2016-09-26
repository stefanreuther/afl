/**
  *  \file afl/net/http/dispatcher.hpp
  *  \brief Interface afl::net::http::Dispatcher
  */
#ifndef AFL_AFL_NET_HTTP_DISPATCHER_HPP
#define AFL_AFL_NET_HTTP_DISPATCHER_HPP

#include <memory>
#include "afl/base/deletable.hpp"

namespace afl { namespace net { namespace http {

    class Response;
    class Request;

    /** HTTP Server Request Dispatcher.
        Instances of this interface take a HTTP request and create a matching response. */
    class Dispatcher : public afl::base::Deletable {
     public:
        /** Create response.

            If this function recognizes the request, it must return a newly-allocated Response instance.
            It can take ownership of the Request if it desires (but does not have to).

            If this function does not recognize the request, it must return null, and leave the Request intact.

            \param request [in/out] Original HTTP request.
            \return Response, if any */
        virtual Response* createNewResponse(std::auto_ptr<Request>& request) = 0;
    };

} } }

#endif
