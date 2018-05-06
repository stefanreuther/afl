/**
  *  \file afl/net/http/cgihandler.hpp
  *  \brief Class afl::net::http::CGIHandler
  */
#ifndef AFL_AFL_NET_HTTP_CGIHANDLER_HPP
#define AFL_AFL_NET_HTTP_CGIHANDLER_HPP

#include "afl/sys/environment.hpp"
#include "afl/io/stream.hpp"

namespace afl { namespace net { namespace http {

    class Page;
    class PageRequest;
    class PageResponse;

    /** CGI Page handler.
        This class allows providing a single Page instance as a CGI program.
        To use, implement a main function as
        <code>
           int main(int, char** argv)
           {
               MyPage p;
               return CGIHandler(afl::sys::Environment::getInstance(argv)).handlePage(p);
           }
        </code>

        This will
        - forward headers provided by the server to the Page
        - perform initial parameter validation and parsing of a possible POST payload
        - forward page output back to the server
        - catch exceptions thrown by page processing and turn them into a simple "500" error page */
    class CGIHandler {
     public:
        /** Constructor.
            \param env Environment */
        CGIHandler(afl::sys::Environment& env);

        /** Destructor. */
        ~CGIHandler();

        /** Handle a page.
            \param page Page to process. */
        int handlePage(Page& page);

     private:
        afl::sys::Environment& m_environment;

        void buildPageRequest(PageRequest& req);
        void sendHeaders(PageResponse& resp, afl::io::Stream& out);
    };

} } }

#endif
