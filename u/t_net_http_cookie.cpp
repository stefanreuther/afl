/**
  *  \file u/t_net_http_cookie.cpp
  *  \brief Test for afl::net::http::Cookie
  */

#include "afl/net/http/cookie.hpp"

#include "u/t_net_http.hpp"
#include "afl/sys/parsedtime.hpp"

/** Simple tests. */
void
TestNetHttpCookie::testIt()
{
    using afl::net::http::Cookie;
    using afl::net::HeaderField;

    // Preparation
    static const afl::sys::ParsedTime pt = { 2001, 9, 9, 1, 46, 40, 0, 0 };
    const afl::sys::Time now(pt, afl::sys::Time::UniversalTime);
    TS_ASSERT_EQUALS(now.getUnixTime(), 1000000000);

    afl::net::Url url;
    TS_ASSERT(url.parse("http://www.example.com/admin/login.cgi?mode=text/html"));

    // Default-initialisation
    {
        Cookie c;
        TS_ASSERT(!c.isValid());
    }

    // A simple cookie
    {
        Cookie c(url, now, HeaderField("session", "12345"));
        TS_ASSERT(!c.isPersistent());
        TS_ASSERT(c.matchUrl(url));
        TS_ASSERT_EQUALS(c.getPath(), "/admin");
        TS_ASSERT_EQUALS(c.getHost(), "www.example.com");
        TS_ASSERT_EQUALS(c.getName(), "session");
        TS_ASSERT_EQUALS(c.getValue(), "12345");
    }

    // Attributes
    {
        Cookie c(url, now, HeaderField("session", "12345; max-age=60; path=/; domain=example.com"));
        TS_ASSERT(c.isPersistent());
        TS_ASSERT(c.matchUrl(url));
        TS_ASSERT_EQUALS(c.getPath(), "/");
        TS_ASSERT_EQUALS(c.getHost(), "example.com");
        TS_ASSERT_EQUALS(c.getName(), "session");
        TS_ASSERT_EQUALS(c.getValue(), "12345");
        TS_ASSERT(!c.isExpired(now));
        TS_ASSERT(c.isExpired(now + afl::sys::Duration::fromSeconds(61)));
        TS_ASSERT_EQUALS(c.toString(), "example.com\tTRUE\t/\tFALSE\t1000000060\tsession\t12345");
    }

    // Attributes
    {
        Cookie c(url, now, HeaderField("session", "12345; max-age=120; secure; httponly"));
        TS_ASSERT(c.isPersistent());
        TS_ASSERT(!c.matchUrl(url));     // because URL is not HTTPS
        TS_ASSERT_EQUALS(c.getPath(), "/admin");
        TS_ASSERT_EQUALS(c.getHost(), "www.example.com");
        TS_ASSERT_EQUALS(c.getName(), "session");
        TS_ASSERT_EQUALS(c.getValue(), "12345");
        TS_ASSERT(!c.isExpired(now));
        TS_ASSERT(!c.isExpired(now + afl::sys::Duration::fromSeconds(119)));
        TS_ASSERT(c.isExpired(now + afl::sys::Duration::fromSeconds(121)));
        TS_ASSERT_EQUALS(c.toString(), "www.example.com\tFALSE\t/admin\tTRUE\t1000000120\tsession\t12345");

        afl::net::Url u2(url);
        u2.setScheme("https");
        TS_ASSERT(c.matchUrl(u2));
    }

    // Immediately-expired cookie
    {
        Cookie c(url, now, HeaderField("session", "12345; max-age=0"));
        TS_ASSERT(c.isPersistent());
        TS_ASSERT(c.matchUrl(url));
        TS_ASSERT_EQUALS(c.getPath(), "/admin");
        TS_ASSERT_EQUALS(c.getHost(), "www.example.com");
        TS_ASSERT_EQUALS(c.getName(), "session");
        TS_ASSERT_EQUALS(c.getValue(), "12345");
        TS_ASSERT(c.isExpired(now));
    }

    // Some path matching
    {
        Cookie c(url, now, HeaderField("session", "12345; path=/admin"));
        TS_ASSERT(!c.isPersistent());
        TS_ASSERT(c.matchUrl(url));

        afl::net::Url other;
        TS_ASSERT(other.parse("http://www.example.com/admin/"));
        TS_ASSERT(c.matchUrl(other));
        TS_ASSERT(other.parse("http://www.example.com/admin/manage.cgi"));
        TS_ASSERT(c.matchUrl(other));
        TS_ASSERT(other.parse("http://www.example.com/admin/subdir/"));
        TS_ASSERT(c.matchUrl(other));

        TS_ASSERT(other.parse("http://www.example.com/admin"));  // does not match because its implied path is "/"
        TS_ASSERT(!c.matchUrl(other));
        TS_ASSERT(other.parse("http://www.example.com/admin2"));
        TS_ASSERT(!c.matchUrl(other));
        TS_ASSERT(other.parse("http://www.example.com/a"));
        TS_ASSERT(!c.matchUrl(other));
        TS_ASSERT(other.parse("http://www.example.com/"));
        TS_ASSERT(!c.matchUrl(other));
        TS_ASSERT(other.parse("http://www.example.com/badmin"));
        TS_ASSERT(!c.matchUrl(other));
    }

    // Some path matching with trailing slash
    {
        Cookie c(url, now, HeaderField("session", "12345; path=/admin/"));
        TS_ASSERT(!c.isPersistent());
        TS_ASSERT(c.matchUrl(url));

        afl::net::Url other;
        TS_ASSERT(other.parse("http://www.example.com/admin"));
        TS_ASSERT(!c.matchUrl(other));
        TS_ASSERT(other.parse("http://www.example.com/admin/"));
        TS_ASSERT(c.matchUrl(other));
        TS_ASSERT(other.parse("http://www.example.com/admin/manage.cgi"));
        TS_ASSERT(c.matchUrl(other));
        TS_ASSERT(other.parse("http://www.example.com/admin/subdir/"));
        TS_ASSERT(c.matchUrl(other));

        TS_ASSERT(other.parse("http://www.example.com/admin2"));
        TS_ASSERT(!c.matchUrl(other));
        TS_ASSERT(other.parse("http://www.example.com/a"));
        TS_ASSERT(!c.matchUrl(other));
        TS_ASSERT(other.parse("http://www.example.com/"));
        TS_ASSERT(!c.matchUrl(other));
        TS_ASSERT(other.parse("http://www.example.com/badmin"));
        TS_ASSERT(!c.matchUrl(other));
    }

    // Some domain matching for unique host
    {
        Cookie c(url, now, HeaderField("session", "12345; path=/admin"));
        TS_ASSERT(!c.isPersistent());
        TS_ASSERT(c.matchUrl(url));

        afl::net::Url other;
        TS_ASSERT(other.parse("http://www.example.com/admin/"));
        TS_ASSERT(c.matchUrl(other));
        TS_ASSERT(other.parse("http://www.example.com:99/admin/"));
        TS_ASSERT(c.matchUrl(other));

        // Wrong host
        TS_ASSERT(other.parse("http://example.com/admin/"));
        TS_ASSERT(!c.matchUrl(other));
        TS_ASSERT(other.parse("http://admin.www.example.com/admin/"));
        TS_ASSERT(!c.matchUrl(other));
    }

    // Some domain matching for domain match
    {
        Cookie c(url, now, HeaderField("session", "12345; domain=example.com"));
        TS_ASSERT(!c.isPersistent());
        TS_ASSERT(c.matchUrl(url));

        afl::net::Url other;
        TS_ASSERT(other.parse("http://www.example.com/admin/"));
        TS_ASSERT(c.matchUrl(other));
        TS_ASSERT(other.parse("http://www.example.com:99/admin/"));
        TS_ASSERT(c.matchUrl(other));
        TS_ASSERT(other.parse("http://example.com/admin/"));
        TS_ASSERT(c.matchUrl(other));
        TS_ASSERT(other.parse("http://admin.www.example.com/admin/"));
        TS_ASSERT(c.matchUrl(other));

        // Nonmatch
        TS_ASSERT(other.parse("http://elsewhere.com/admin"));
        TS_ASSERT(!c.matchUrl(other));
        TS_ASSERT(other.parse("http://com/admin"));
        TS_ASSERT(!c.matchUrl(other));
    }

    // Expire date
    {
        Cookie c(url, now, HeaderField("session", "12345; expires=Sun, 9 Sep 01:46:50 GMT 2001"));
        TS_ASSERT(c.isPersistent());
        TS_ASSERT(c.matchUrl(url));
        TS_ASSERT(!c.isExpired(now));
        TS_ASSERT(!c.isExpired(now + afl::sys::Duration::fromSeconds(9)));
        TS_ASSERT(c.isExpired(now + afl::sys::Duration::fromSeconds(11)));
    }
    {
        Cookie c(url, now, HeaderField("session", "12345; expires=Sun, 09 Sep 2001 01:46:50 GMT"));
        TS_ASSERT(c.isPersistent());
        TS_ASSERT(c.matchUrl(url));
        TS_ASSERT(!c.isExpired(now));
        TS_ASSERT(!c.isExpired(now + afl::sys::Duration::fromSeconds(9)));
        TS_ASSERT(c.isExpired(now + afl::sys::Duration::fromSeconds(11)));
    }
    {
        Cookie c(url, now, HeaderField("session", "12345; expires=Sun, 09 Sep 2001 01:46:50 GMT; max-age=20"));
        TS_ASSERT(c.isPersistent());
        TS_ASSERT(c.matchUrl(url));
        TS_ASSERT(!c.isExpired(now));
        TS_ASSERT(!c.isExpired(now + afl::sys::Duration::fromSeconds(9)));
        TS_ASSERT(!c.isExpired(now + afl::sys::Duration::fromSeconds(11)));
        TS_ASSERT(!c.isExpired(now + afl::sys::Duration::fromSeconds(19)));
        TS_ASSERT(c.isExpired(now + afl::sys::Duration::fromSeconds(21)));
    }
    {
        Cookie c(url, now, HeaderField("session", "12345; expires=Sun, 09 Sep 01 01:46:50 GMT"));
        TS_ASSERT(c.isPersistent());
        TS_ASSERT(c.matchUrl(url));
        TS_ASSERT(!c.isExpired(now));
        TS_ASSERT(!c.isExpired(now + afl::sys::Duration::fromSeconds(9)));
        TS_ASSERT(c.isExpired(now + afl::sys::Duration::fromSeconds(11)));
    }

    // IP addresses
    {
        afl::net::Url ip;
        TS_ASSERT(ip.parse("http://123.45.67.8/"));

        afl::net::Url ip2;
        TS_ASSERT(ip.parse("http://55.45.67.8/"));

        Cookie c(ip, now, HeaderField("a", "b; domain=45.67.8"));
        TS_ASSERT(!c.isPersistent());
        TS_ASSERT(!c.matchUrl(ip));
        TS_ASSERT(!c.matchUrl(ip2));

        Cookie c2(ip, now, HeaderField("a", "b"));
        TS_ASSERT(!c2.isPersistent());
        TS_ASSERT(c2.matchUrl(ip));
        TS_ASSERT(!c2.matchUrl(ip2));
    }

    // Parsed from cookies.txt
    {
        Cookie c;
        TS_ASSERT(c.parseFromString("www.example.com\tFALSE\t/admin\tTRUE\t1000000120\tsession\t12345"));
        TS_ASSERT(c.isPersistent());
        TS_ASSERT_EQUALS(c.getPath(), "/admin");
        TS_ASSERT_EQUALS(c.getHost(), "www.example.com");
        TS_ASSERT_EQUALS(c.getName(), "session");
        TS_ASSERT_EQUALS(c.getValue(), "12345");
        TS_ASSERT(!c.isExpired(now));
        TS_ASSERT(!c.isExpired(now + afl::sys::Duration::fromSeconds(119)));
        TS_ASSERT(c.isExpired(now + afl::sys::Duration::fromSeconds(121)));

        TS_ASSERT(!c.parseFromString("# Comment"));
        TS_ASSERT(!c.parseFromString("www.example.com FALSE /admin TRUE 1000000120 session 12345"));
        TS_ASSERT(!c.parseFromString("www.example.com\tFALSE\t/admin\tTRUE\t1000000120"));
        TS_ASSERT(!c.parseFromString("www.example.com\tFALSE\t/admin\tTRUE\ttoday\tsession\t12345"));
    }
}
