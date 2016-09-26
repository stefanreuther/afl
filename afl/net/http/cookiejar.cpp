/**
  *  \file afl/net/http/cookiejar.cpp
  *  \brief Class afl::net::http::CookieJar
  */

#include <algorithm>
#include "afl/net/http/cookiejar.hpp"
#include "afl/net/headerconsumer.hpp"
#include "afl/sys/time.hpp"
#include "afl/sys/mutexguard.hpp"
#include "afl/io/textfile.hpp"
#include "afl/charset/utf8charset.hpp"

namespace {
    bool cookieCompare(const afl::net::http::Cookie* a, const afl::net::http::Cookie* b)
    {
        // RFC 6265 5.4 step 2: cookies with longer paths go first;
        // among equal paths, older cookie goes first.
        // Age is modelled using sequence numbers.
        if (a->getPath().size() == b->getPath().size()) {
            return a->getSequenceNumber() < b->getSequenceNumber();
        } else {
            return a->getPath().size() > b->getPath().size();
        }
    }
}

afl::net::http::CookieJar::CookieJar()
    : m_mutex(),
      m_cookies(),
      m_counter(0)
{ }

afl::net::http::CookieJar::~CookieJar()
{ }

void
afl::net::http::CookieJar::addCookie(const Cookie& cookie)
{
    afl::sys::MutexGuard g(m_mutex);

    // Check whether we are updating an existing cookie
    bool found = false;
    for (std::vector<Cookie>::iterator it = m_cookies.begin(), end = m_cookies.end(); it != end; ++it) {
        if (it->matchCookie(cookie)) {
            // OK, found it. Preserve the serial number.
            uint32_t seq = it->getSequenceNumber();
            *it = cookie;
            it->setSequenceNumber(seq);
            found = true;
            break;
        }
    }

    // Make a new cookie if we didn't update an existing one
    if (!found) {
        m_cookies.push_back(cookie);
        m_cookies.back().setSequenceNumber(++m_counter);
    }
}

void
afl::net::http::CookieJar::removeCookie(const Cookie& cookie)
{
    afl::sys::MutexGuard g(m_mutex);
    for (std::vector<Cookie>::iterator it = m_cookies.begin(), end = m_cookies.end(); it != end; ++it) {
        if (it->matchCookie(cookie)) {
            // Found the cookie. There can be at most one, so there's no need to continue the loop.
            m_cookies.erase(it);
            break;
        }
    }
}

void
afl::net::http::CookieJar::addCookies(const Url& requestUrl, const HeaderTable& cookies)
{
    afl::sys::MutexGuard g(m_mutex);

    class CookieAdder : public HeaderConsumer {
     public:
        CookieAdder(CookieJar& jar, const Url& url)
            : m_jar(jar),
              m_url(url),
              m_now(afl::sys::Time::getCurrentTime())
            { }

        void handleHeader(String_t name, String_t value)
            {
                Cookie cookie(m_url, m_now, HeaderField(name, value));
                if (cookie.matchUrl(m_url)) {
                    // The cookie matches the request URL, so we treat it as valid
                    if (cookie.isExpired(m_now)) {
                        // The cookie is immediately expired, so delete it
                        m_jar.removeCookie(cookie);
                    } else {
                        // New/updated cookie
                        m_jar.addCookie(cookie);
                    }
                }
            }
     private:
        CookieJar& m_jar;
        const Url& m_url;
        const afl::sys::Time m_now;
    };

    CookieAdder adder(*this, requestUrl);
    cookies.enumerateHeaders(adder);
}

void
afl::net::http::CookieJar::loadCookies(afl::io::Stream& in)
{
    afl::sys::MutexGuard g(m_mutex);

    // Read the file as UTF-8 (=untranslated)
    afl::io::TextFile tf(in);
    tf.setCharsetNew(new afl::charset::Utf8Charset());

    // Query time once to avoid non-atomicity
    afl::sys::Time now = afl::sys::Time::getCurrentTime();

    // Read and add all valid cookies
    String_t line;
    while (tf.readLine(line)) {
        Cookie c;
        if (c.parseFromString(line) && !c.isExpired(now)) {
            addCookie(c);
        }
    }
}

void
afl::net::http::CookieJar::saveCookies(afl::io::Stream& out) const
{
    afl::sys::MutexGuard g(m_mutex);

    // Write the file as UTF-8 (=untranslated)
    afl::io::TextFile tf(out);
    tf.setCharsetNew(new afl::charset::Utf8Charset());

    // Save all persistent cookies
    for (std::vector<Cookie>::const_iterator it = m_cookies.begin(), end = m_cookies.end(); it != end; ++it) {
        if (it->isPersistent()) {
            tf.writeLine(it->toString());
        }
    }
    tf.flush();
}

void
afl::net::http::CookieJar::enumerateCookies(const Url& requestUrl, HeaderConsumer& consumer) const
{
    afl::sys::MutexGuard g(m_mutex);

    // Enumerate the matching cookies
    std::vector<const Cookie*> cookies;
    afl::sys::Time now = afl::sys::Time::getCurrentTime();
    for (std::vector<Cookie>::const_iterator it = m_cookies.begin(), end = m_cookies.end(); it != end; ++it) {
        if (it->matchUrl(requestUrl) && !it->isExpired(now)) {
            cookies.push_back(&*it);
        }
    }

    if (!cookies.empty()) {
        // Sort them
        std::sort(cookies.begin(), cookies.end(), cookieCompare);

        // Produce output
        for (std::vector<const Cookie*>::const_iterator it = cookies.begin(), end = cookies.end(); it != end; ++it) {
            consumer.handleHeader((**it).getName(), (**it).getValue());
        }
    }
}
