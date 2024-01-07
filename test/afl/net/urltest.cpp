/**
  *  \file test/afl/net/urltest.cpp
  *  \brief Test for afl::net::Url
  */

#include "afl/net/url.hpp"

#include "afl/net/headerconsumer.hpp"
#include "afl/test/testrunner.hpp"

/*
 *  Test URL parsing.
 */
AFL_TEST("afl.net.Url:parse:empty", a)
{
    afl::net::Url u;
    a.check("parse",            u.parse(""));
    a.checkEqual("getScheme",   u.getScheme(), "");
    a.checkEqual("getUser",     u.getUser(), "");
    a.checkEqual("getPassword", u.getPassword(), "");
    a.checkEqual("getHost",     u.getHost(), "");
    a.checkEqual("getPort",     u.getPort(), "");
    a.checkEqual("getPath",     u.getPath(), "");
    a.checkEqual("getFragment", u.getFragment(), "");
    a.check("isNull",           u.isNull());
    a.check("isValid",         !u.isValid());
    a.check("isValidHost",     !u.isValidHost());
    a.check("isValidId",       !u.isValidId());
    a.checkEqual("toString",    u.toString(), "");
}

AFL_TEST("afl.net.Url:parse:name", a)
{
    afl::net::Url u;
    a.check("parse",            u.parse("a"));
    a.checkEqual("getScheme",   u.getScheme(), "");
    a.checkEqual("getUser",     u.getUser(), "");
    a.checkEqual("getPassword", u.getPassword(), "");
    a.checkEqual("getHost",     u.getHost(), "");
    a.checkEqual("getPort",     u.getPort(), "");
    a.checkEqual("getPath",     u.getPath(), "a");
    a.checkEqual("getFragment", u.getFragment(), "");
    a.check("isNull",          !u.isNull());
    a.check("isValid",         !u.isValid());
    a.check("isValidHost",     !u.isValidHost());
    a.check("isValidId",       !u.isValidId());
    a.checkEqual("toString",    u.toString(), "a");
}

AFL_TEST("afl.net.Url:parse:all-parts", a)
{
    afl::net::Url u;
    a.check("parse",                 u.parse("ftp://user:pass@host:1234/foo/bar#frag"));
    a.checkEqual("getScheme",        u.getScheme(), "ftp");
    a.checkEqual("getUser",          u.getUser(), "user");
    a.checkEqual("getPassword",      u.getPassword(), "pass");
    a.checkEqual("getHost",          u.getHost(), "host");
    a.checkEqual("getPort",          u.getPort(), "1234");
    a.checkEqual("getPath",          u.getPath(), "/foo/bar");
    a.checkEqual("getFragment",      u.getFragment(), "#frag");
    a.check("isNull",               !u.isNull());
    a.check("isValid",               u.isValid());
    a.check("isValidHost",           u.isValidHost());
    a.check("isValidId",            !u.isValidId());
    a.checkEqual("getName.toString", u.getName("80").toString(), "host:1234");
    a.checkEqual("toString",         u.toString(), "ftp://user:pass@host:1234/foo/bar#frag");
}

AFL_TEST("afl.net.Url:parse:all-parts-escaped", a)
{
    afl::net::Url u;
    a.check("parse",                 u.parse("a%61a://b%62b:c%63c@d%64d:1234/foo%2fbar#e%65e"));
    a.checkEqual("getScheme",        u.getScheme(), "aaa");
    a.checkEqual("getUser",          u.getUser(), "bbb");
    a.checkEqual("getPassword",      u.getPassword(), "ccc");
    a.checkEqual("getHost",          u.getHost(), "ddd");
    a.checkEqual("getPort",          u.getPort(), "1234");
    a.checkEqual("getPath",          u.getPath(), "/foo%2fbar");
    a.checkEqual("getFragment",      u.getFragment(), "#eee");
    a.checkEqual("getName.toString", u.getName("42").toString(), "ddd:1234");
    a.check("isNull",               !u.isNull());
    a.check("isValid",               u.isValid());
    a.check("isValidHost",           u.isValidHost());
    a.check("isValidId",            !u.isValidId());
    a.checkEqual("toString",         u.toString(), "aaa://bbb:ccc@ddd:1234/foo%2fbar#eee");
}

AFL_TEST("afl.net.Url:parse:just-host+frag", a)
{
    afl::net::Url u;
    a.check("parse",                 u.parse("//host#frag"));
    a.checkEqual("getScheme",        u.getScheme(), "");
    a.checkEqual("getUser",          u.getUser(), "");
    a.checkEqual("getPassword",      u.getPassword(), "");
    a.checkEqual("getHost",          u.getHost(), "host");
    a.checkEqual("getPort",          u.getPort(), "");
    a.checkEqual("getPath",          u.getPath(), "/");
    a.checkEqual("getFragment",      u.getFragment(), "#frag");
    a.check("isNull",               !u.isNull());
    a.checkEqual("getName.toString", u.getName("80").toString(), "host:80");
    a.check("isValid",              !u.isValid());        // we don't have a scheme
    a.check("isValidHost",          !u.isValidHost());
    a.check("isValidId",            !u.isValidId());
    a.checkEqual("toString",         u.toString(), "//host/#frag");
}

AFL_TEST("afl.net.Url:parse:just-path+frag", a)
{
    afl::net::Url u;
    a.check("parse",            u.parse("/path#frag"));
    a.checkEqual("getScheme",   u.getScheme(), "");
    a.checkEqual("getUser",     u.getUser(), "");
    a.checkEqual("getPassword", u.getPassword(), "");
    a.checkEqual("getHost",     u.getHost(), "");
    a.checkEqual("getPort",     u.getPort(), "");
    a.checkEqual("getPath",     u.getPath(), "/path");
    a.checkEqual("getFragment", u.getFragment(), "#frag");
    a.check("isNull",          !u.isNull());
    a.check("isValid",         !u.isValid());
    a.check("isValidHost",     !u.isValidHost());
    a.check("isValidId",       !u.isValidId());
    a.checkEqual("toString",    u.toString(), "/path#frag");
}

AFL_TEST("afl.net.Url:parse:ipv6", a)
{
    afl::net::Url u;
    a.check("parse",                 u.parse("http://[::1]:20/xyz"));
    a.checkEqual("getScheme",        u.getScheme(), "http");
    a.checkEqual("getUser",          u.getUser(), "");
    a.checkEqual("getPassword",      u.getPassword(), "");
    a.checkEqual("getHost",          u.getHost(), "::1");
    a.checkEqual("getPort",          u.getPort(), "20");
    a.checkEqual("getPath",          u.getPath(), "/xyz");
    a.checkEqual("getFragment",      u.getFragment(), "");
    a.check("isNull",               !u.isNull());
    a.check("isValid",               u.isValid());
    a.check("isValidHost",           u.isValidHost());
    a.check("isValidId",            !u.isValidId());
    a.checkEqual("getName.toString", u.getName("80").toString(), "[::1]:20");
    a.checkEqual("toString",         u.toString(), "http://[::1]:20/xyz");
}

AFL_TEST("afl.net.Url:parse:just-scheme", a)
{
    afl::net::Url u;
    a.check("parse",            u.parse("file:///"));
    a.checkEqual("getScheme",   u.getScheme(), "file");
    a.checkEqual("getUser",     u.getUser(), "");
    a.checkEqual("getPassword", u.getPassword(), "");
    a.checkEqual("getHost",     u.getHost(), "");
    a.checkEqual("getPort",     u.getPort(), "");
    a.checkEqual("getPath",     u.getPath(), "/");
    a.checkEqual("getFragment", u.getFragment(), "");
    a.check("isNull",          !u.isNull());
    a.check("isValid",          u.isValid());

    // Since we don't have a host part, this URL behaves interestingly (but consistently).
    // The Url class treats it as an Id URL with a path of "/".
    a.check("isValidHost",     !u.isValidHost());
    a.check("isValidId",        u.isValidId());
    a.checkEqual("toString",    u.toString(), "file:/");
}

AFL_TEST("afl.net.Url:parse:scheme+ipv6", a)
{
    afl::net::Url u;
    a.check("parse",                 u.parse("foo://[::1]"));
    a.checkEqual("getScheme",        u.getScheme(), "foo");
    a.checkEqual("getUser",          u.getUser(), "");
    a.checkEqual("getPassword",      u.getPassword(), "");
    a.checkEqual("getHost",          u.getHost(), "::1");
    a.checkEqual("getPort",          u.getPort(), "");
    a.checkEqual("getPath",          u.getPath(), "/");
    a.checkEqual("getFragment",      u.getFragment(), "");
    a.check("isNull",               !u.isNull());
    a.check("isValid",               u.isValid());
    a.check("isValidHost",           u.isValidHost());
    a.check("isValidId",            !u.isValidId());
    a.checkEqual("getName.toString", u.getName("80").toString(), "[::1]:80");
    a.checkEqual("toString",         u.toString(), "foo://[::1]/");
}

AFL_TEST("afl.net.Url:parse:just-fragment", a)
{
    afl::net::Url u;
    a.check("parse",            u.parse("#hi"));
    a.checkEqual("getScheme",   u.getScheme(), "");
    a.checkEqual("getUser",     u.getUser(), "");
    a.checkEqual("getPassword", u.getPassword(), "");
    a.checkEqual("getHost",     u.getHost(), "");
    a.checkEqual("getPort",     u.getPort(), "");
    a.checkEqual("getPath",     u.getPath(), "");
    a.checkEqual("getFragment", u.getFragment(), "#hi");
    a.check("isNull",          !u.isNull());
    a.check("isValid",         !u.isValid());
    a.check("isValidHost",     !u.isValidHost());
    a.check("isValidId",       !u.isValidId());
    a.checkEqual("toString",    u.toString(), "#hi");
}

AFL_TEST("afl.net.Url:parse:relative-path", a)
{
    afl::net::Url u;
    a.check("parse",            u.parse("../foo"));
    a.checkEqual("getScheme",   u.getScheme(), "");
    a.checkEqual("getUser",     u.getUser(), "");
    a.checkEqual("getPassword", u.getPassword(), "");
    a.checkEqual("getHost",     u.getHost(), "");
    a.checkEqual("getPort",     u.getPort(), "");
    a.checkEqual("getPath",     u.getPath(), "../foo");
    a.checkEqual("getFragment", u.getFragment(), "");
    a.check("isNull",          !u.isNull());
    a.check("isValid",         !u.isValid());
    a.check("isValidHost",     !u.isValidHost());
    a.check("isValidId",       !u.isValidId());
    a.checkEqual("toString",    u.toString(), "../foo");
}

AFL_TEST("afl.net.Url:parse:urn", a)
{
    afl::net::Url u;
    a.check("parse",            u.parse("urn:bla:bla:bla"));
    a.checkEqual("getScheme",   u.getScheme(), "urn");
    a.checkEqual("getUser",     u.getUser(), "");
    a.checkEqual("getPassword", u.getPassword(), "");
    a.checkEqual("getHost",     u.getHost(), "");
    a.checkEqual("getPort",     u.getPort(), "");
    a.checkEqual("getPath",     u.getPath(), "bla:bla:bla");
    a.checkEqual("getFragment", u.getFragment(), "");
    a.check("isNull",          !u.isNull());
    a.check("isValid",          u.isValid());
    a.check("isValidHost",     !u.isValidHost());
    a.check("isValidId",        u.isValidId());
    a.checkEqual("toString",    u.toString(), "urn:bla:bla:bla");
}

AFL_TEST("afl.net.Url:parse:dlna", a)
{
    // DLNA PlayContainer URIs are pretty nonstandard.
    // Our class will parse them, but the result will be far from optimal.
    afl::net::Url u;
    a.check("parse",            u.parse("dlna-playcontainer://urn%3a1234?sid=urn%3afoo&cid=0"));
    a.checkEqual("getScheme",   u.getScheme(), "dlna-playcontainer");
    a.checkEqual("getUser",     u.getUser(), "");
    a.checkEqual("getPassword", u.getPassword(), "");
    a.checkEqual("getHost",     u.getHost(), "urn:1234");
    a.checkEqual("getPort",     u.getPort(), "");
    a.checkEqual("getPath",     u.getPath(), "/?sid=urn%3afoo&cid=0");
    a.checkEqual("getFragment", u.getFragment(), "");
    a.check("isNull",          !u.isNull());
    a.check("isValid",          u.isValid());
    a.check("isValidHost",      u.isValidHost());
    a.check("isValidId",       !u.isValidId());
    a.checkEqual("toString",    u.toString(), "dlna-playcontainer://[urn:1234]/?sid=urn%3afoo&cid=0");
}

AFL_TEST("afl.net.Url:parse:mailto", a)
{
    afl::net::Url u;
    a.check("parse",            u.parse("mailto:user@dom.ain"));
    a.checkEqual("getScheme",   u.getScheme(), "mailto");
    a.checkEqual("getUser",     u.getUser(), "");
    a.checkEqual("getPassword", u.getPassword(), "");
    a.checkEqual("getHost",     u.getHost(), "");
    a.checkEqual("getPort",     u.getPort(), "");
    a.checkEqual("getPath",     u.getPath(), "user@dom.ain");
    a.checkEqual("getFragment", u.getFragment(), "");
    a.check("isNull",          !u.isNull());
    a.check("isValid",          u.isValid());
    a.check("isValidHost",     !u.isValidHost());
    a.check("isValidId",        u.isValidId());
    a.checkEqual("toString",    u.toString(), "mailto:user@dom.ain");
}

AFL_TEST("afl.net.Url:parse:data", a)
{
    afl::net::Url u;
    a.check("parse",            u.parse("data:image/gif;base64,R0lGODlhFgAWAID/AMDAwAAAACH5BAEAAAAALAAAAAAWABYAQAInhI+pa+H9mJy0LhdgtrxzDG5WGFVk6aXqyk6Y9kXvKKNuLbb6zgMFADs="));
    a.checkEqual("getScheme",   u.getScheme(), "data");
    a.checkEqual("getUser",     u.getUser(), "");
    a.checkEqual("getPassword", u.getPassword(), "");
    a.checkEqual("getHost",     u.getHost(), "");
    a.checkEqual("getPort",     u.getPort(), "");
    a.checkEqual("getPath",     u.getPath().substr(0,20), "image/gif;base64,R0l");
    a.checkEqual("getFragment", u.getFragment(), "");
}

AFL_TEST("afl.net.Url:parse:errors", a)
{
    afl::net::Url u;
    // a.check("", !u.parse(":")); Not invalid; not recognized as a null scheme
    // a.check("", !u.parse("foo:")); Not invalid.
    a.check("host+ques",        !u.parse("http://[::1]?bar"));

    // Syntax error examples
    a.check("no-host",          !u.parse("foo://:@"));
    a.check("no-ipv6",          !u.parse("foo://["));
    a.check("host+name",        !u.parse("foo://[::1]bla"));
    a.check("no-port",          !u.parse("foo://host:/"));
    a.check("port-not-numeric", !u.parse("foo://host:123bla"));
}

/*
 *  URL merging.
 */

AFL_TEST("afl.net.Url:mergeFrom:fragment", a)
{
    afl::net::Url u, u2;
    a.check("parse", u.parse("http://foo/bar"));
    a.check("parse2", u2.parse("#baz"));
    u2.mergeFrom(u);
    a.checkEqual("getScheme",   u2.getScheme(), "http");
    a.checkEqual("getUser",     u2.getUser(), "");
    a.checkEqual("getPassword", u2.getPassword(), "");
    a.checkEqual("getHost",     u2.getHost(), "foo");
    a.checkEqual("getPort",     u2.getPort(), "");
    a.checkEqual("getPath",     u2.getPath(), "/bar");
    a.checkEqual("getFragment", u2.getFragment(), "#baz");
}

AFL_TEST("afl.net.Url:mergeFrom:file-name", a)
{
    afl::net::Url u, u2;
    a.check("parse", u.parse("http://foo/bar"));
    a.check("parse2", u2.parse("baz"));
    u2.mergeFrom(u);
    a.checkEqual("getScheme",   u2.getScheme(), "http");
    a.checkEqual("getUser",     u2.getUser(), "");
    a.checkEqual("getPassword", u2.getPassword(), "");
    a.checkEqual("getHost",     u2.getHost(), "foo");
    a.checkEqual("getPort",     u2.getPort(), "");
    a.checkEqual("getPath",     u2.getPath(), "/baz");
    a.checkEqual("getFragment", u2.getFragment(), "");
}

AFL_TEST("afl.net.Url:mergeFrom:dir+file-name", a)
{
    afl::net::Url u, u2;
    a.check("parse", u.parse("http://foo/bar/"));
    a.check("parse2", u2.parse("baz"));
    u2.mergeFrom(u);
    a.checkEqual("getScheme",   u2.getScheme(), "http");
    a.checkEqual("getUser",     u2.getUser(), "");
    a.checkEqual("getPassword", u2.getPassword(), "");
    a.checkEqual("getHost",     u2.getHost(), "foo");
    a.checkEqual("getPort",     u2.getPort(), "");
    a.checkEqual("getPath",     u2.getPath(), "/bar/baz");
    a.checkEqual("getFragment", u2.getFragment(), "");
}

AFL_TEST("afl.net.Url:mergeFrom:relative-name", a)
{
    afl::net::Url u, u2;
    a.check("parse", u.parse("http://foo/bar/baz/narf#boo"));
    a.check("parse2", u2.parse("../fred"));
    u2.mergeFrom(u);
    a.checkEqual("getScheme",   u2.getScheme(), "http");
    a.checkEqual("getUser",     u2.getUser(), "");
    a.checkEqual("getPassword", u2.getPassword(), "");
    a.checkEqual("getHost",     u2.getHost(), "foo");
    a.checkEqual("getPort",     u2.getPort(), "");
    a.checkEqual("getPath",     u2.getPath(), "/bar/fred");
    a.checkEqual("getFragment", u2.getFragment(), "");
}

AFL_TEST("afl.net.Url:mergeFrom:just-up", a)
{
    afl::net::Url u, u2;
    a.check("parse", u.parse("http://foo/bar/baz/narf#boo"));
    a.check("parse2", u2.parse(".."));
    u2.mergeFrom(u);
    a.checkEqual("getScheme",   u2.getScheme(), "http");
    a.checkEqual("getUser",     u2.getUser(), "");
    a.checkEqual("getPassword", u2.getPassword(), "");
    a.checkEqual("getHost",     u2.getHost(), "foo");
    a.checkEqual("getPort",     u2.getPort(), "");
    a.checkEqual("getPath",     u2.getPath(), "/bar");
    a.checkEqual("getFragment", u2.getFragment(), "");
}

AFL_TEST("afl.net.Url:mergeFrom:up-from-root", a)
{
    afl::net::Url u, u2;
    a.check("parse", u.parse("http://foo/"));
    a.check("parse2", u2.parse(".."));
    u2.mergeFrom(u);
    a.checkEqual("getScheme",   u2.getScheme(), "http");
    a.checkEqual("getUser",     u2.getUser(), "");
    a.checkEqual("getPassword", u2.getPassword(), "");
    a.checkEqual("getHost",     u2.getHost(), "foo");
    a.checkEqual("getPort",     u2.getPort(), "");
    a.checkEqual("getPath",     u2.getPath(), "/");
    a.checkEqual("getFragment", u2.getFragment(), "");
}

AFL_TEST("afl.net.Url:mergeFrom:server", a)
{
    afl::net::Url u, u2;
    a.check("parse", u.parse("http://foo:77/"));
    a.check("parse2", u2.parse("//bar#1"));
    u2.mergeFrom(u);
    a.checkEqual("getScheme",   u2.getScheme(), "http");
    a.checkEqual("getUser",     u2.getUser(), "");
    a.checkEqual("getPassword", u2.getPassword(), "");
    a.checkEqual("getHost",     u2.getHost(), "bar");
    a.checkEqual("getPort",     u2.getPort(), "");
    a.checkEqual("getPath",     u2.getPath(), "/");
    a.checkEqual("getFragment", u2.getFragment(), "#1");
}


AFL_TEST("afl.net.Url:set", a)
{
    afl::net::Url u;
    a.check("parse",            u.parse(""));
    a.checkEqual("getScheme",   u.getScheme(), "");
    a.checkEqual("getUser",     u.getUser(), "");
    a.checkEqual("getPassword", u.getPassword(), "");
    a.checkEqual("getHost",     u.getHost(), "");
    a.checkEqual("getPort",     u.getPort(), "");
    a.checkEqual("getPath",     u.getPath(), "");
    a.checkEqual("getFragment", u.getFragment(), "");
    a.check("isNull",           u.isNull());
    a.check("isValid",         !u.isValid());
    a.check("isValidHost",     !u.isValidHost());
    a.check("isValidId",       !u.isValidId());
    a.checkEqual("toString",    u.toString(), "");

    u.setHost("host");
    a.checkEqual("updated getHost", u.getHost(), "host");

    u.setPort("80");
    a.checkEqual("updated getPort", u.getPort(), "80");

    u.setPassword("s3cr3t");
    a.checkEqual("updated getPassword", u.getPassword(), "s3cr3t");

    u.setUser("root");
    a.checkEqual("updated getUser", u.getUser(), "root");

    u.setScheme("http");
    a.checkEqual("updated getScheme", u.getScheme(), "http");

    u.setPath("/login");
    a.checkEqual("updated getPath", u.getPath(), "/login");

    u.setFragment("#top");
    a.checkEqual("updated getFragment", u.getFragment(), "#top");

    a.checkEqual("updated toString", u.toString(), "http://root:s3cr3t@host:80/login#top");
}

/** Test afl::net::matchPath(). */
AFL_TEST("afl.net.Url:matchPath", a)
{
    using afl::net::matchPath;

    String_t sub;
    a.check("01. matchPath", matchPath("/dir/sub", "", sub));
    a.checkEqual("01. sub", sub, "/dir/sub");

    a.check("02. matchPath", matchPath("/dir/sub", "/dir", sub));
    a.checkEqual("02. sub", sub, "/sub");

    a.check("03. matchPath", matchPath("/dir/sub", "/dir/sub", sub));
    a.checkEqual("03. sub", sub, "");

    a.check("04. matchPath", !matchPath("/dir/sub", "/", sub));
    a.check("05. matchPath", !matchPath("/dir/sub", "/di", sub));
    a.check("06. matchPath", !matchPath("/dir/sub", "/dir/sub/sub", sub));

    a.check("07. matchPath", matchPath("/dir/sub?q", "", sub));
    a.checkEqual("07. sub", sub, "/dir/sub?q");

    a.check("08. matchPath", matchPath("/dir/sub?q", "/dir", sub));
    a.checkEqual("08. sub", sub, "/sub?q");

    a.check("09. matchPath", matchPath("/dir/sub?q", "/dir/sub", sub));
    a.checkEqual("09. sub", sub, "?q");

    a.check("10. matchPath", !matchPath("/dir/sub?q", "/", sub));
    a.check("11. matchPath", !matchPath("/dir/sub?q", "/di", sub));
    a.check("12. matchPath", !matchPath("/dir/sub?q", "/dir/sub/sub", sub));
}

/** Test afl::net::matchArguments(). */
AFL_TEST("afl.net.Url:matchArguments", a)
{
    class TestHeaderConsumer : public afl::net::HeaderConsumer {
     public:
        virtual void handleHeader(String_t key, String_t value)
            {
                m_result += "<<";
                m_result += key;
                m_result += ">>";
                m_result += "((";
                m_result += value;
                m_result += "))";
            }
        String_t m_result;
    };
    TestHeaderConsumer thc;
    a.checkEqual("01. result", thc.m_result, "");

    String_t path = "/foo";
    afl::net::matchArguments(path, thc);
    a.checkEqual("02. path", path, "/foo");
    a.checkEqual("02. result", thc.m_result, "");

    thc.m_result = "";
    path = "/foo?";
    afl::net::matchArguments(path, thc);
    a.checkEqual("03. path", path, "/foo");
    a.checkEqual("03. result", thc.m_result, "");

    thc.m_result = "";
    path = "/foo?x";
    afl::net::matchArguments(path, thc);
    a.checkEqual("04. path", path, "/foo");
    a.checkEqual("04. result", thc.m_result, "<<x>>(())");

    thc.m_result = "";
    path = "/foo?x&y&";
    afl::net::matchArguments(path, thc);
    a.checkEqual("05. path", path, "/foo");
    a.checkEqual("05. result", thc.m_result, "<<x>>(())<<y>>(())");

    thc.m_result = "";
    path = "/foo?x=y";
    afl::net::matchArguments(path, thc);
    a.checkEqual("06. path", path, "/foo");
    a.checkEqual("06. result", thc.m_result, "<<x>>((y))");

    thc.m_result = "";
    path = "/foo?x=y&z";
    afl::net::matchArguments(path, thc);
    a.checkEqual("07. path", path, "/foo");
    a.checkEqual("07. result", thc.m_result, "<<x>>((y))<<z>>(())");

    thc.m_result = "";
    path = "/foo?x=a+b+c";
    afl::net::matchArguments(path, thc);
    a.checkEqual("08. path", path, "/foo");
    a.checkEqual("08. result", thc.m_result, "<<x>>((a b c))");

    thc.m_result = "";
    path = "/foo?=a";
    afl::net::matchArguments(path, thc);
    a.checkEqual("09. path", path, "/foo");
    a.checkEqual("09. result", thc.m_result, "<<>>((a))");

    thc.m_result = "";
    path = "/foo?a=%3D%25";
    afl::net::matchArguments(path, thc);
    a.checkEqual("10. path", path, "/foo");
    a.checkEqual("10. result", thc.m_result, "<<a>>((=%))");

    thc.m_result = "";
    path = "/foo?%3D=%25";
    afl::net::matchArguments(path, thc);
    a.checkEqual("11. path", path, "/foo");
    a.checkEqual("11. result", thc.m_result, "<<=>>((%))");

    thc.m_result = "";
    path = "/foo?p=/foo";
    afl::net::matchArguments(path, thc);
    a.checkEqual("12. path", path, "/foo");
    a.checkEqual("12. result", thc.m_result, "<<p>>((/foo))");

    thc.m_result = "";
    path = "/foo?p=/foo?p=/foo";
    afl::net::matchArguments(path, thc);
    a.checkEqual("13. path", path, "/foo");
    a.checkEqual("13. result", thc.m_result, "<<p>>((/foo?p=/foo))");

    thc.m_result = "";
    path = "/foo?p=/foo?p=/foo&a=1";
    afl::net::matchArguments(path, thc);
    a.checkEqual("14. path", path, "/foo");
    a.checkEqual("14. result", thc.m_result, "<<p>>((/foo?p=/foo))<<a>>((1))");
}
