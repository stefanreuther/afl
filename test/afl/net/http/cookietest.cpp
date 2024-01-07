/**
  *  \file test/afl/net/http/cookietest.cpp
  *  \brief Test for afl::net::http::Cookie
  */

#include "afl/net/http/cookie.hpp"

#include "afl/sys/parsedtime.hpp"
#include "afl/test/testrunner.hpp"

using afl::net::HeaderField;
using afl::net::Url;
using afl::net::http::Cookie;

namespace {
    Url makeUrl(afl::test::Assert a)
    {
        Url url;
        a.check("url valid", url.parse("http://www.example.com/admin/login.cgi?mode=text/html"));
        return url;
    }

    afl::sys::Time makeTime(afl::test::Assert a)
    {
        static const afl::sys::ParsedTime pt = { 2001, 9, 9, 1, 46, 40, 0, 0 };
        const afl::sys::Time now(pt, afl::sys::Time::UniversalTime);
        a.checkEqual("parsed time value", now.getUnixTime(), 1000000000);
        return now;
    }

    Cookie make(afl::test::Assert a, String_t value)
    {
        return Cookie(makeUrl(a), makeTime(a), HeaderField("session", value));
    }
}

// Default-initialisation
AFL_TEST("afl.net.http.Cookie:default", a)
{
    Cookie c;
    a.check("", !c.isValid());
}

// A simple cookie
AFL_TEST("afl.net.http.Cookie:default", a)
{
    Cookie c(make(a, "12345"));
    a.check("isPersistent", !c.isPersistent());
    a.check("matchUrl", c.matchUrl(makeUrl(a)));
    a.checkEqual("getPath", c.getPath(), "/admin");
    a.checkEqual("getHost", c.getHost(), "www.example.com");
    a.checkEqual("getName", c.getName(), "session");
    a.checkEqual("getValue", c.getValue(), "12345");
}

// Attributes
AFL_TEST("afl.net.http.Cookie:attributes", a)
{
    Cookie c(make(a, "12345; max-age=60; path=/; domain=example.com"));
    a.check("isPersistent", c.isPersistent());
    a.check("matchUrl", c.matchUrl(makeUrl(a)));
    a.checkEqual("getPath", c.getPath(), "/");
    a.checkEqual("getHost", c.getHost(), "example.com");
    a.checkEqual("getName", c.getName(), "session");
    a.checkEqual("getValue", c.getValue(), "12345");
    a.check("isExpired now", !c.isExpired(makeTime(a)));
    a.check("isExpired later", c.isExpired(makeTime(a) + afl::sys::Duration::fromSeconds(61)));
    a.checkEqual("toString", c.toString(), "example.com\tTRUE\t/\tFALSE\t1000000060\tsession\t12345");
}

// Attributes
AFL_TEST("afl.net.http.Cookie:attributes2", a)
{
    Cookie c(make(a, "12345; max-age=120; secure; httponly"));
    a.check("isPersistent", c.isPersistent());
    a.check("matchUrl", !c.matchUrl(makeUrl(a)));     // because URL is not HTTPS
    a.checkEqual("getPath", c.getPath(), "/admin");
    a.checkEqual("getHost", c.getHost(), "www.example.com");
    a.checkEqual("getName", c.getName(), "session");
    a.checkEqual("getValue", c.getValue(), "12345");
    a.check("isExpired now", !c.isExpired(makeTime(a)));
    a.check("isExpired soon", !c.isExpired(makeTime(a) + afl::sys::Duration::fromSeconds(119)));
    a.check("isExpired later", c.isExpired(makeTime(a) + afl::sys::Duration::fromSeconds(121)));
    a.checkEqual("toString", c.toString(), "www.example.com\tFALSE\t/admin\tTRUE\t1000000120\tsession\t12345");

    afl::net::Url u2(makeUrl(a));
    u2.setScheme("https");
    a.check("matchUrl secure", c.matchUrl(u2));
}

// Immediately-expired cookie
AFL_TEST("afl.net.http.Cookie:expired", a)
{
    Cookie c(make(a, "12345; max-age=0"));
    a.check("isPersistent", c.isPersistent());
    a.check("matchUrl", c.matchUrl(makeUrl(a)));
    a.checkEqual("getPath", c.getPath(), "/admin");
    a.checkEqual("getHost", c.getHost(), "www.example.com");
    a.checkEqual("getName", c.getName(), "session");
    a.checkEqual("getValue", c.getValue(), "12345");
    a.check("isExpired", c.isExpired(makeTime(a)));
}

// Some path matching
AFL_TEST("afl.net.http.Cookie:matchUrl:file", a)
{
    Cookie c(make(a, "12345; path=/admin"));
    a.check("isPersistent", !c.isPersistent());
    a.check("matchUrl", c.matchUrl(makeUrl(a)));

    afl::net::Url other;
    a.check("1. parse", other.parse("http://www.example.com/admin/"));
    a.check("1. matchUrl", c.matchUrl(other));
    a.check("2. parse", other.parse("http://www.example.com/admin/manage.cgi"));
    a.check("2. matchUrl", c.matchUrl(other));
    a.check("3. parse", other.parse("http://www.example.com/admin/subdir/"));
    a.check("3. matchUrl", c.matchUrl(other));

    a.check("4. parse", other.parse("http://www.example.com/admin"));  // does not match because its implied path is "/"
    a.check("4. matchUrl", !c.matchUrl(other));
    a.check("5. parse", other.parse("http://www.example.com/admin2"));
    a.check("5. matchUrl", !c.matchUrl(other));
    a.check("6. parse", other.parse("http://www.example.com/a"));
    a.check("6. matchUrl", !c.matchUrl(other));
    a.check("7. parse", other.parse("http://www.example.com/"));
    a.check("7. matchUrl", !c.matchUrl(other));
    a.check("8. parse", other.parse("http://www.example.com/badmin"));
    a.check("8. matchUrl", !c.matchUrl(other));
}

// Some path matching with trailing slash
AFL_TEST("afl.net.http.Cookie:matchUrl:dir", a)
{
    Cookie c(make(a, "12345; path=/admin/"));
    a.check("isPersistent", !c.isPersistent());
    a.check("matchUrl", c.matchUrl(makeUrl(a)));

    afl::net::Url other;
    a.check("1. parse", other.parse("http://www.example.com/admin"));
    a.check("1. matchUrl", !c.matchUrl(other));
    a.check("2. parse", other.parse("http://www.example.com/admin/"));
    a.check("2. matchUrl", c.matchUrl(other));
    a.check("3. parse", other.parse("http://www.example.com/admin/manage.cgi"));
    a.check("3. matchUrl", c.matchUrl(other));
    a.check("4. parse", other.parse("http://www.example.com/admin/subdir/"));
    a.check("4. matchUrl", c.matchUrl(other));

    a.check("5. parse", other.parse("http://www.example.com/admin2"));
    a.check("5. matchUrl", !c.matchUrl(other));
    a.check("6. parse", other.parse("http://www.example.com/a"));
    a.check("6. matchUrl", !c.matchUrl(other));
    a.check("7. parse", other.parse("http://www.example.com/"));
    a.check("7. matchUrl", !c.matchUrl(other));
    a.check("8. parse", other.parse("http://www.example.com/badmin"));
    a.check("8. matchUrl", !c.matchUrl(other));
}

// Some domain matching for unique host
AFL_TEST("afl.net.http.Cookie:matchUrl:default-host", a)
{
    Cookie c(make(a, "12345; path=/admin"));
    a.check("isPersistent", !c.isPersistent());
    a.check("matchUrl", c.matchUrl(makeUrl(a)));

    afl::net::Url other;
    a.check(". parse", other.parse("http://www.example.com/admin/"));
    a.check(". matchUrl", c.matchUrl(other));
    a.check(". parse", other.parse("http://www.example.com:99/admin/"));
    a.check(". matchUrl", c.matchUrl(other));

    // Wrong host
    a.check(". parse", other.parse("http://example.com/admin/"));
    a.check(". matchUrl", !c.matchUrl(other));
    a.check(". parse", other.parse("http://admin.www.example.com/admin/"));
    a.check(". matchUrl", !c.matchUrl(other));
}

// Some domain matching for domain match
AFL_TEST("afl.net.http.Cookie:matchUrl:explicit-domain", a)
{
    Cookie c(make(a, "12345; domain=example.com"));
    a.check("isPersistent", !c.isPersistent());
    a.check("matchUrl", c.matchUrl(makeUrl(a)));

    afl::net::Url other;
    a.check("1. parse", other.parse("http://www.example.com/admin/"));
    a.check("1. matchUrl", c.matchUrl(other));
    a.check("2. parse", other.parse("http://www.example.com:99/admin/"));
    a.check("2. matchUrl", c.matchUrl(other));
    a.check("3. parse", other.parse("http://example.com/admin/"));
    a.check("3. matchUrl", c.matchUrl(other));
    a.check("4. parse", other.parse("http://admin.www.example.com/admin/"));
    a.check("4. matchUrl", c.matchUrl(other));

    // Nonmatch
    a.check("5. parse", other.parse("http://elsewhere.com/admin"));
    a.check("5. matchUrl", !c.matchUrl(other));
    a.check("6. parse", other.parse("http://com/admin"));
    a.check("6. matchUrl", !c.matchUrl(other));
}

// Expire date
AFL_TEST("afl.net.http.Cookie:expires", a)
{
    Cookie c(make(a, "12345; expires=Sun, 9 Sep 01:46:50 GMT 2001"));
    a.check("isPersistent", c.isPersistent());
    a.check("matchUrl", c.matchUrl(makeUrl(a)));
    a.check("isExpired now", !c.isExpired(makeTime(a)));
    a.check("isExpired soon", !c.isExpired(makeTime(a) + afl::sys::Duration::fromSeconds(9)));
    a.check("isExpired later", c.isExpired(makeTime(a) + afl::sys::Duration::fromSeconds(11)));
}

AFL_TEST("afl.net.http.Cookie:expires:variant", a)
{
    Cookie c(make(a, "12345; expires=Sun, 09 Sep 2001 01:46:50 GMT"));
    a.check("isPersistent", c.isPersistent());
    a.check("matchUrl", c.matchUrl(makeUrl(a)));
    a.check("isExpired now", !c.isExpired(makeTime(a)));
    a.check("isExpired soon", !c.isExpired(makeTime(a) + afl::sys::Duration::fromSeconds(9)));
    a.check("isExpired later", c.isExpired(makeTime(a) + afl::sys::Duration::fromSeconds(11)));
}

AFL_TEST("afl.net.http.Cookie:expires:date+max-age", a)
{
    Cookie c(make(a, "12345; expires=Sun, 09 Sep 2001 01:46:50 GMT; max-age=20"));
    a.check("isPersistent", c.isPersistent());
    a.check("matchUrl", c.matchUrl(makeUrl(a)));
    a.check("isExpired now", !c.isExpired(makeTime(a)));
    a.check("isExpired +9", !c.isExpired(makeTime(a) + afl::sys::Duration::fromSeconds(9)));
    a.check("isExpired +11", !c.isExpired(makeTime(a) + afl::sys::Duration::fromSeconds(11)));
    a.check("isExpired +19", !c.isExpired(makeTime(a) + afl::sys::Duration::fromSeconds(19)));
    a.check("isExpired +21", c.isExpired(makeTime(a) + afl::sys::Duration::fromSeconds(21)));
}

AFL_TEST("afl.net.http.Cookie:expires:implicit-century", a)
{
    Cookie c(make(a, "12345; expires=Sun, 09 Sep 01 01:46:50 GMT"));
    a.check("isPersistent", c.isPersistent());
    a.check("matchUrl", c.matchUrl(makeUrl(a)));
    a.check("isExpired now", !c.isExpired(makeTime(a)));
    a.check("isExpired soon", !c.isExpired(makeTime(a) + afl::sys::Duration::fromSeconds(9)));
    a.check("isExpired later", c.isExpired(makeTime(a) + afl::sys::Duration::fromSeconds(11)));
}

// IP addresses
AFL_TEST("afl.net.http.Cookie:ip-as-domain", a)
{
    afl::net::Url ip;
    a.check("parse 1", ip.parse("http://123.45.67.8/"));

    afl::net::Url ip2;
    a.check("parse 2", ip.parse("http://55.45.67.8/"));

    Cookie c(ip, makeTime(a), HeaderField("a", "b; domain=45.67.8"));
    a.check("isPersistent", !c.isPersistent());
    a.check("matchUrl 1", !c.matchUrl(ip));
    a.check("matchUrl 2", !c.matchUrl(ip2));

    Cookie c2(ip, makeTime(a), HeaderField("a", "b"));
    a.check("cookie 2 isPersistent", !c2.isPersistent());
    a.check("cookie 2 matchUrl 1", c2.matchUrl(ip));
    a.check("cookie 2 matchUrl 2", !c2.matchUrl(ip2));
}

// Parsed from cookies.txt
AFL_TEST("afl.net.http.Cookie:parseFromString", a)
{
    Cookie c;
    a.check("parseFromString", c.parseFromString("www.example.com\tFALSE\t/admin\tTRUE\t1000000120\tsession\t12345"));
    a.check("isPersistent", c.isPersistent());
    a.checkEqual("getPath", c.getPath(), "/admin");
    a.checkEqual("getHost", c.getHost(), "www.example.com");
    a.checkEqual("getName", c.getName(), "session");
    a.checkEqual("getValue", c.getValue(), "12345");
    a.check("isExpired now", !c.isExpired(makeTime(a)));
    a.check("isExpired soon", !c.isExpired(makeTime(a) + afl::sys::Duration::fromSeconds(119)));
    a.check("isExpired later", c.isExpired(makeTime(a) + afl::sys::Duration::fromSeconds(121)));

    a.check("parseFromString: comment",       !c.parseFromString("# Comment"));
    a.check("parseFromString: no-tab",        !c.parseFromString("www.example.com FALSE /admin TRUE 1000000120 session 12345"));
    a.check("parseFromString: missing-field", !c.parseFromString("www.example.com\tFALSE\t/admin\tTRUE\t1000000120"));
    a.check("parseFromString: type-error",    !c.parseFromString("www.example.com\tFALSE\t/admin\tTRUE\ttoday\tsession\t12345"));
}
