/**
  *  \file afl/net/http/cookiejar.hpp
  *  \brief Class afl::net::http::CookieJar
  */
#ifndef AFL_AFL_NET_HTTP_COOKIEJAR_HPP
#define AFL_AFL_NET_HTTP_COOKIEJAR_HPP

#include <vector>
#include "afl/net/http/cookie.hpp"
#include "afl/io/stream.hpp"
#include "afl/sys/mutex.hpp"
#include "afl/net/url.hpp"
#include "afl/net/headertable.hpp"
#include "afl/net/headerconsumer.hpp"

namespace afl { namespace net { namespace http {

    /** HTTP Cookie Jar.
        Stores a set of cookies and provides operations on them.

        This class is fully interlocked, and can be used safely from multiple threads.
        This class uses to current system time (afl::sys::Time::getCurrentTime())
        to determine cookie expiration. */
    class CookieJar {
     public:
        /** Constructor.
            Makes an empty cookie jar. */
        CookieJar();

        /** Destructor. */
        ~CookieJar();

        /** Add a cookie.
            The cookie is either added anew, or replaces an existing cookie for the same name and URL.
            \param cookie The cookie */
        void addCookie(const Cookie& cookie);

        /** Remove a cookie.
            If a cookie of the same name and URL already exists, it is removed.
            \param cookie The cookie */
        void removeCookie(const Cookie& cookie);

        /** Add cookies from a HTTP request.
            \param requestUrl Original request URL
            \param cookies Cookies received from the server */
        void addCookies(const Url& requestUrl, const HeaderTable& cookies);

        /** Load cookies from cookies.txt.
            \param in Stream pointing at a cookies.txt file open for reading */
        void loadCookies(afl::io::Stream& in);

        /** Save cookies to cookies.txt.
            \param out Stream pointing at a cookies.txt file open for writing */
        void saveCookies(afl::io::Stream& out) const;

        /** Enumerate cookies for a request.
            This filters all cookies that match the request and invokes your callback.
            \param requestUrl Request URL
            \param consumer Matching cookies will be enumerated here */
        void enumerateCookies(const Url& requestUrl, HeaderConsumer& consumer) const;

     private:
        mutable afl::sys::Mutex m_mutex;
        std::vector<Cookie> m_cookies;
        uint32_t m_counter;
    };

} } }

#endif
