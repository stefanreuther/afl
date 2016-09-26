/**
  *  \file afl/net/http/page.hpp
  *  \brief Interface afl::net::http::Page
  */
#ifndef AFL_AFL_NET_HTTP_PAGE_HPP
#define AFL_AFL_NET_HTTP_PAGE_HPP

#include "afl/base/deletable.hpp"
#include "afl/string/string.hpp"

namespace afl { namespace net { namespace http {

    class PageRequest;
    class PageResponse;

    /** Web page.
        This class provides a simpler abstraction than Response for implementing HTTP responses.
        It takes input in a PageRequest object, and produces output in a PageResponse object.
        It therefore need not deal with HTTP protocol specifics such as having to generate headers or not,
        or parsing form uploads.
        A Page cannot only be attached to a HTTP server (afl::net::http::ProtocolHandler),
        but can also be used to implement other protocols such as CGI or FCGI.

        On the downside, it is not useful to use for huge downloads or uploads
        because it buffers the whole request and response message.
        If you need that, use Response.

        To hook a set of Page's to a HTTP server, use PageDispatcher. */
    class Page : public afl::base::Deletable {
     public:
        /** Check for valid method.
            \param method Request method, in upper case.
            \return true if this method is allowed */
        virtual bool isValidMethod(const String_t& method) const = 0;

        /** Check for valid path argument.
            \return true if this page accepts a "path" argument appended to its name */
        virtual bool isValidPath() const = 0;

        /** Handle request.
            This method performs the logic of this page.
            \param in Original request. You are allowed to modify it.
            \param out Result */
        virtual void handleRequest(PageRequest& in, PageResponse& out) = 0;
    };

} } }

#endif
