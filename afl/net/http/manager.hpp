/**
  *  \file afl/net/http/manager.hpp
  *  \brief Class afl::net::http::Manager
  */
#ifndef AFL_AFL_NET_HTTP_MANAGER_HPP
#define AFL_AFL_NET_HTTP_MANAGER_HPP

#include "afl/net/url.hpp"
#include "afl/net/headertable.hpp"

namespace afl { namespace net { namespace http {

    class Client;
    class DownloadListener;
    class CookieJar;

    /** HTTP Manager.
        This class serves as a central point of configuration for HTTP,
        and includes methods to simplify building HTTP requests
        for typical ("API", "browser") use-cases. */
    class Manager {
     public:
        Manager(Client& client);

        /*
         *  Configuration
         */

        // FIXME: getProxy, setProxy
        // FIXME: user-agent
        // FIXME: configure Accept: gzip and other user headers

        void setCookieJar(CookieJar& cookies);


        /*
         *  Actions
         */

        /** Get a file. Posts a GET request for the given URL.
            \param url URL to get
            \param listener Download listener, will receive callbacks from the HTTP client thread
            \return request Id */
        uint32_t getFile(const Url& url, DownloadListener& listener);

        /** Post.
            \param url URL to post to
            \param data Data to post
            \param dataType Content-Type of data
            \param listener Download listener, will receive callbacks from the HTTP client thread
            \return request Id */
        uint32_t postFile(const Url& url, String_t data, String_t dataType, DownloadListener& listener);

        // FIXME: getFileHeader(Url, ...)
        // FIXME: getFileRange(Url, Start, End, ...)
        // FIXME: getForm(Url, Params, ...)
        // FIXME: postForm(Url, Params, ...)

        void cancelRequest(uint32_t id);

        Client& client();

        /*
         *  Request Processing Semi-Internals
         */

        String_t getHeaders(const Url& url);

        void addCookies(const Url& url, const HeaderTable& cookies);

     private:
        /** HTTP client. */
        Client& m_client;

        /** URL-scheme-to-port-number mapping.
            A HeaderTable behaves just like we need. */
        HeaderTable m_portTable;

        /** Cookie jar. */
        CookieJar* m_pCookies;

        Url normalizeUrl(Url url);
    };

} } }

#endif
