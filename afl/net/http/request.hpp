/**
  *  \file afl/net/http/request.hpp
  *  \brief Class afl::net::http::Request
  */
#ifndef AFL_AFL_NET_HTTP_REQUEST_HPP
#define AFL_AFL_NET_HTTP_REQUEST_HPP

#include "afl/net/headerconsumer.hpp"
#include "afl/net/headerparser.hpp"
#include "afl/net/headertable.hpp"

namespace afl { namespace net { namespace http {

    /** HTTP Request Parser.
        Parses a HTTP request including its Request-Line and the request headers.
        It supports HTTP/0.9, HTTP/1.0 and HTTP/1.1.

        Usage:
        - push data into it using handleData() until it reports completion
        - use getter methods to access the parsed request */
    class Request {
     public:
        /** Constructor.
            Makes a blank request. */
        Request();

        /** Push header data.
            \param name [in] Identification of the data source (e.g. peer name), for error reporting.
            \param data [in/out] On input, data to process. On output, remaining unprocessed data.
            \retval false More data required (%data is empty)
            \retval true No more data required (%data has unprocessed data) */
        bool handleData(const String_t& name, afl::base::ConstBytes_t& bytes);

        /** Get request method ("HEAD", "GET", "POST", etc.).
            The method is reported as is, and is interpreted case-sensitively (RfC 2616 5.1.1).
            \return request method */
        const String_t& getMethod() const;

        /** Get path ("/").
            The path is reported as is.
            \return path */
        const String_t& getPath() const;

        /** Get version ("HTTP/1.0").
            The intended use of this field is to report it back in the Response-Line.
            \return version */
        const String_t& getVersion() const;

        /** Check for error.
            \retval false header parsed successfully
            \retval true header contained syntactic errors and could not be parsed */
        bool hasErrors() const;

        /** Check for path match.
            The path is considered matching if it is
            - equal to the prefix
            - starts with the prefix, followed by a suffix starting with "/" or "?"
            See afl::net::matchPath for details.

            \param prefix [in] Expected path prefix
            \param suffix [out] Path suffix
            \retval true match succeeded; \c suffix has been set
            \retval false match failed */
        bool matchPath(const String_t& prefix, String_t& suffix) const;

        /** Get headers.
            \return headers */
        const HeaderTable& headers() const;

        /** Get headers, modifiable.
            \return headers */
        HeaderTable& headers();

        /** Check for keepalive.
            This function checks whether the client wishes to use persistent connections.
            The client may report not being able to support that; the Response can opt out as well.
            \return keepalive flag */
        bool isKeepalive() const;

        /** Check for response header.
            \return true if the client wants a response header. HTTP/0.9 clients do not want one. */
        bool isResponseHeaderRequested() const;

        /** Check for response body.
            This function checks whether the client wishes to receive a response body;
            it sends a HEAD request if it does not want one.
            The server may also generate response codes that report that it has no body.
            \return true if the client wants a response body. */
        bool isResponseBodyRequested() const;

     private:
        String_t m_method;
        String_t m_path;
        String_t m_version;
        HeaderTable m_header;
        HeaderParser m_headerParser;

        enum State {
            Method,
            MethodSp,
            Path,
            PathSp,
            Version,
            Header,
            Done,
            Error
        };
        State m_state;
    };

} } }

#endif
