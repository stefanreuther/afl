/**
  *  \file afl/net/http/pagerequest.hpp
  *  \brief Class afl::net::http::PageRequest
  */
#ifndef AFL_AFL_NET_HTTP_PAGEREQUEST_HPP
#define AFL_AFL_NET_HTTP_PAGEREQUEST_HPP

#include "afl/net/headertable.hpp"
#include "afl/string/string.hpp"
#include "afl/io/internalsink.hpp"
#include "afl/net/http/formparser.hpp"

namespace afl { namespace net { namespace http {

    class Request;

    /** Parameters for a Page request.
        Objects of this class contain input data for a Page::handleRequest() call.

        An object of this class is typically configured by the PageDispatcher,
        before being given to a Page that uses accessor methods to access the data.
        Because it is no longer needed after being used by the Page,
        Page may also modify it if it sees fit. */
    class PageRequest {
     public:
        /** Constructor.
            \param rootPath Root path. If nonempty, should start but not end in "/".
            \param selfPath Path to page. If nonempty, should start but not end in "/".
            \param path Path passed to page as parameter.

            For example, when we're serving pages under "/root" (argument to PageDispatcher constructor),
            have a page at "/page", and the user asks for "/root/page/sub?p=1",
            the PageDispatcher will pass rootPath="/root", selfPath="/page", and path="/sub?p=1".
            PageRequest will automatically parse the query string as form arguments. */
        PageRequest(String_t rootPath, String_t selfPath, String_t path);

        /** Set body data to be ignored.
            This function is called when it's known from other request parameters that a possible request message body can be ignored.
            After this function call, handleData() will simply discard data instead of parsing or storing it. */
        void setIgnoreData();

        /** Process body data.
            Depending on the request message headers, this will parse the data as form argument,
            store it away for later retrieval in body(), or discard it. */
        void handleData(afl::base::ConstBytes_t data);

        /** Finalize.
            Call this when the object has been completely configured.
            This performs necessary post-processing. */
        void finish();

        /** Access request headers.
            \return message headers */
        const HeaderTable& headers() const;

        /** Access request headers, mutable version.
            \return message headers */
        HeaderTable& headers();

        /** Access request body.
            If the request contained a non-form payload, it can be accessed with this function.
            Form payloads can be accessed through arguments().
            \return request body */
        const afl::io::InternalSink& body() const;

        /** Access request body, mutable version.
            If the request contained a non-form payload, it can be accessed with this function.
            Form payloads can be accessed through arguments().
            \return request body */
        afl::io::InternalSink& body();

        /** Access form arguments.
            FIXME: document behaviour for file names etc.
            \return form arguments */
        const HeaderTable& arguments() const;

        /** Access form arguments, mutable version.
            FIXME: document behaviour for file names etc.
            \return form arguments */
        HeaderTable& arguments();

        /** Set request method.
            \param method Method (upper-case, UTF-8) */
        void setMethod(const String_t& method);

        /** Get request method.
            \return Method (upper-case, UTF-8) */
        const String_t& getMethod() const;

        /** Set root path.
            See constructor for details.
            \param rootPath new root path */
        void setRootPath(const String_t& rootPath);

        /** Get root path.
            See constructor for details.
            \return root path */
        const String_t& getRootPath() const;

        /** Set page path.
            See constructor for details.
            \param selfPath new self path */
        void setSelfPath(const String_t& selfPath);

        /** Get page path.
            See constructor for details.
            \return self path */
        const String_t& getSelfPath() const;

        /** Set path argument.
            See constructor for details.
            \param path new path argument */
        void setPath(const String_t& path);

        /** Get path argumetn.
            See constructor for details.
            \return path argument */
        const String_t& getPath() const;

     private:
        // Original headers
        HeaderTable m_headers;

        // Upload body
        afl::io::InternalSink m_body;

        // GET and POST form arguments
        HeaderTable m_arguments;

        // FIXME: cookies

        String_t m_method;

        // Original path:
        //     /foo/bar.htm/baz?a=1&b=2&c=33
        //     |  ||      ||  ||           |
        //     root  self  path  arguments
        String_t m_rootPath;
        String_t m_selfPath;
        String_t m_path;

        enum InputStatus {
            Undecided,
            Ignore,
            SaveBody,
            ParseForm
        };
        InputStatus m_inputStatus;

        FormParser m_formParser;
    };

} } }

#endif
