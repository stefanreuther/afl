/**
  *  \file u/t_net_url.cpp
  *  \brief Test for afl::net::Url
  */

#include "u/t_net.hpp"
#include "afl/net/url.hpp"
#include "afl/net/headerconsumer.hpp"

/** Test URL parsing. */
void
TestNetUrl::testParse()
{
    {
        afl::net::Url u;
        TS_ASSERT(u.parse(""));
        TS_ASSERT_EQUALS(u.getScheme(), "");
        TS_ASSERT_EQUALS(u.getUser(), "");
        TS_ASSERT_EQUALS(u.getPassword(), "");
        TS_ASSERT_EQUALS(u.getHost(), "");
        TS_ASSERT_EQUALS(u.getPort(), "");
        TS_ASSERT_EQUALS(u.getPath(), "");
        TS_ASSERT_EQUALS(u.getFragment(), "");
        TS_ASSERT(u.isNull());
        TS_ASSERT(!u.isValid());
        TS_ASSERT(!u.isValidHost());
        TS_ASSERT(!u.isValidId());
        TS_ASSERT_EQUALS(u.toString(), "");
    }
    {
        afl::net::Url u;
        TS_ASSERT(u.parse("a"));
        TS_ASSERT_EQUALS(u.getScheme(), "");
        TS_ASSERT_EQUALS(u.getUser(), "");
        TS_ASSERT_EQUALS(u.getPassword(), "");
        TS_ASSERT_EQUALS(u.getHost(), "");
        TS_ASSERT_EQUALS(u.getPort(), "");
        TS_ASSERT_EQUALS(u.getPath(), "a");
        TS_ASSERT_EQUALS(u.getFragment(), "");
        TS_ASSERT(!u.isNull());
        TS_ASSERT(!u.isValid());
        TS_ASSERT(!u.isValidHost());
        TS_ASSERT(!u.isValidId());
        TS_ASSERT_EQUALS(u.toString(), "a");
    }
    {
        afl::net::Url u;
        TS_ASSERT(u.parse("ftp://user:pass@host:1234/foo/bar#frag"));
        TS_ASSERT_EQUALS(u.getScheme(), "ftp");
        TS_ASSERT_EQUALS(u.getUser(), "user");
        TS_ASSERT_EQUALS(u.getPassword(), "pass");
        TS_ASSERT_EQUALS(u.getHost(), "host");
        TS_ASSERT_EQUALS(u.getPort(), "1234");
        TS_ASSERT_EQUALS(u.getPath(), "/foo/bar");
        TS_ASSERT_EQUALS(u.getFragment(), "#frag");
        TS_ASSERT(!u.isNull());
        TS_ASSERT( u.isValid());
        TS_ASSERT( u.isValidHost());
        TS_ASSERT(!u.isValidId());
        TS_ASSERT_EQUALS(u.getName("80").toString(), "host:1234");
        TS_ASSERT_EQUALS(u.toString(), "ftp://user:pass@host:1234/foo/bar#frag");
    }
    {
        afl::net::Url u;
        TS_ASSERT(u.parse("a%61a://b%62b:c%63c@d%64d:1234/foo%2fbar#e%65e"));
        TS_ASSERT_EQUALS(u.getScheme(), "aaa");
        TS_ASSERT_EQUALS(u.getUser(), "bbb");
        TS_ASSERT_EQUALS(u.getPassword(), "ccc");
        TS_ASSERT_EQUALS(u.getHost(), "ddd");
        TS_ASSERT_EQUALS(u.getPort(), "1234");
        TS_ASSERT_EQUALS(u.getPath(), "/foo%2fbar");
        TS_ASSERT_EQUALS(u.getFragment(), "#eee");
        TS_ASSERT_EQUALS(u.getName("42").toString(), "ddd:1234");
        TS_ASSERT(!u.isNull());
        TS_ASSERT( u.isValid());
        TS_ASSERT( u.isValidHost());
        TS_ASSERT(!u.isValidId());
        TS_ASSERT_EQUALS(u.toString(), "aaa://bbb:ccc@ddd:1234/foo%2fbar#eee");
    }
    {
        afl::net::Url u;
        TS_ASSERT(u.parse("//host#frag"));
        TS_ASSERT_EQUALS(u.getScheme(), "");
        TS_ASSERT_EQUALS(u.getUser(), "");
        TS_ASSERT_EQUALS(u.getPassword(), "");
        TS_ASSERT_EQUALS(u.getHost(), "host");
        TS_ASSERT_EQUALS(u.getPort(), "");
        TS_ASSERT_EQUALS(u.getPath(), "/");
        TS_ASSERT_EQUALS(u.getFragment(), "#frag");
        TS_ASSERT(!u.isNull());
        TS_ASSERT_EQUALS(u.getName("80").toString(), "host:80");
        TS_ASSERT(!u.isValid());        // we don't have a scheme
        TS_ASSERT(!u.isValidHost());
        TS_ASSERT(!u.isValidId());
        TS_ASSERT_EQUALS(u.toString(), "//host/#frag");
    }
    {
        afl::net::Url u;
        TS_ASSERT(u.parse("/path#frag"));
        TS_ASSERT_EQUALS(u.getScheme(), "");
        TS_ASSERT_EQUALS(u.getUser(), "");
        TS_ASSERT_EQUALS(u.getPassword(), "");
        TS_ASSERT_EQUALS(u.getHost(), "");
        TS_ASSERT_EQUALS(u.getPort(), "");
        TS_ASSERT_EQUALS(u.getPath(), "/path");
        TS_ASSERT_EQUALS(u.getFragment(), "#frag");
        TS_ASSERT(!u.isNull());
        TS_ASSERT(!u.isValid());
        TS_ASSERT(!u.isValidHost());
        TS_ASSERT(!u.isValidId());
        TS_ASSERT_EQUALS(u.toString(), "/path#frag");
    }
    {
        afl::net::Url u;
        TS_ASSERT(u.parse("http://[::1]:20/xyz"));
        TS_ASSERT_EQUALS(u.getScheme(), "http");
        TS_ASSERT_EQUALS(u.getUser(), "");
        TS_ASSERT_EQUALS(u.getPassword(), "");
        TS_ASSERT_EQUALS(u.getHost(), "::1");
        TS_ASSERT_EQUALS(u.getPort(), "20");
        TS_ASSERT_EQUALS(u.getPath(), "/xyz");
        TS_ASSERT_EQUALS(u.getFragment(), "");
        TS_ASSERT(!u.isNull());
        TS_ASSERT( u.isValid());
        TS_ASSERT( u.isValidHost());
        TS_ASSERT(!u.isValidId());
        TS_ASSERT_EQUALS(u.getName("80").toString(), "[::1]:20");
        TS_ASSERT_EQUALS(u.toString(), "http://[::1]:20/xyz");
    }
    {
        afl::net::Url u;
        TS_ASSERT(u.parse("file:///"));
        TS_ASSERT_EQUALS(u.getScheme(), "file");
        TS_ASSERT_EQUALS(u.getUser(), "");
        TS_ASSERT_EQUALS(u.getPassword(), "");
        TS_ASSERT_EQUALS(u.getHost(), "");
        TS_ASSERT_EQUALS(u.getPort(), "");
        TS_ASSERT_EQUALS(u.getPath(), "/");
        TS_ASSERT_EQUALS(u.getFragment(), "");
        TS_ASSERT(!u.isNull());
        TS_ASSERT( u.isValid());

        // Since we don't have a host part, this URL behaves interestingly (but consistently).
        // The Url class treats it as an Id URL with a path of "/".
        TS_ASSERT(!u.isValidHost());
        TS_ASSERT( u.isValidId());
        TS_ASSERT_EQUALS(u.toString(), "file:/");
    }
    {
        afl::net::Url u;
        TS_ASSERT(u.parse("foo://[::1]"));
        TS_ASSERT_EQUALS(u.getScheme(), "foo");
        TS_ASSERT_EQUALS(u.getUser(), "");
        TS_ASSERT_EQUALS(u.getPassword(), "");
        TS_ASSERT_EQUALS(u.getHost(), "::1");
        TS_ASSERT_EQUALS(u.getPort(), "");
        TS_ASSERT_EQUALS(u.getPath(), "/");
        TS_ASSERT_EQUALS(u.getFragment(), "");
        TS_ASSERT(!u.isNull());
        TS_ASSERT( u.isValid());
        TS_ASSERT( u.isValidHost());
        TS_ASSERT(!u.isValidId());
        TS_ASSERT_EQUALS(u.getName("80").toString(), "[::1]:80");
        TS_ASSERT_EQUALS(u.toString(), "foo://[::1]/");
    }
    {
        afl::net::Url u;
        TS_ASSERT(u.parse("#hi"));
        TS_ASSERT_EQUALS(u.getScheme(), "");
        TS_ASSERT_EQUALS(u.getUser(), "");
        TS_ASSERT_EQUALS(u.getPassword(), "");
        TS_ASSERT_EQUALS(u.getHost(), "");
        TS_ASSERT_EQUALS(u.getPort(), "");
        TS_ASSERT_EQUALS(u.getPath(), "");
        TS_ASSERT_EQUALS(u.getFragment(), "#hi");
        TS_ASSERT(!u.isNull());
        TS_ASSERT(!u.isValid());
        TS_ASSERT(!u.isValidHost());
        TS_ASSERT(!u.isValidId());
        TS_ASSERT_EQUALS(u.toString(), "#hi");
    }
    {
        afl::net::Url u;
        TS_ASSERT(u.parse("../foo"));
        TS_ASSERT_EQUALS(u.getScheme(), "");
        TS_ASSERT_EQUALS(u.getUser(), "");
        TS_ASSERT_EQUALS(u.getPassword(), "");
        TS_ASSERT_EQUALS(u.getHost(), "");
        TS_ASSERT_EQUALS(u.getPort(), "");
        TS_ASSERT_EQUALS(u.getPath(), "../foo");
        TS_ASSERT_EQUALS(u.getFragment(), "");
        TS_ASSERT(!u.isNull());
        TS_ASSERT(!u.isValid());
        TS_ASSERT(!u.isValidHost());
        TS_ASSERT(!u.isValidId());
        TS_ASSERT_EQUALS(u.toString(), "../foo");
    }
    {
        afl::net::Url u;
        TS_ASSERT(u.parse("urn:bla:bla:bla"));
        TS_ASSERT_EQUALS(u.getScheme(), "urn");
        TS_ASSERT_EQUALS(u.getUser(), "");
        TS_ASSERT_EQUALS(u.getPassword(), "");
        TS_ASSERT_EQUALS(u.getHost(), "");
        TS_ASSERT_EQUALS(u.getPort(), "");
        TS_ASSERT_EQUALS(u.getPath(), "bla:bla:bla");
        TS_ASSERT_EQUALS(u.getFragment(), "");
        TS_ASSERT(!u.isNull());
        TS_ASSERT( u.isValid());
        TS_ASSERT(!u.isValidHost());
        TS_ASSERT( u.isValidId());
        TS_ASSERT_EQUALS(u.toString(), "urn:bla:bla:bla");
    }
    {
        // DLNA PlayContainer URIs are pretty nonstandard.
        // Our class will parse them, but the result will be far from optimal.
        afl::net::Url u;
        TS_ASSERT(u.parse("dlna-playcontainer://urn%3a1234?sid=urn%3afoo&cid=0"));
        TS_ASSERT_EQUALS(u.getScheme(), "dlna-playcontainer");
        TS_ASSERT_EQUALS(u.getUser(), "");
        TS_ASSERT_EQUALS(u.getPassword(), "");
        TS_ASSERT_EQUALS(u.getHost(), "urn:1234");
        TS_ASSERT_EQUALS(u.getPort(), "");
        TS_ASSERT_EQUALS(u.getPath(), "/?sid=urn%3afoo&cid=0");
        TS_ASSERT_EQUALS(u.getFragment(), "");
        TS_ASSERT(!u.isNull());
        TS_ASSERT( u.isValid());
        TS_ASSERT( u.isValidHost());
        TS_ASSERT(!u.isValidId());
        TS_ASSERT_EQUALS(u.toString(), "dlna-playcontainer://[urn:1234]/?sid=urn%3afoo&cid=0");
    }
    {
        afl::net::Url u;
        TS_ASSERT(u.parse("mailto:user@dom.ain"));
        TS_ASSERT_EQUALS(u.getScheme(), "mailto");
        TS_ASSERT_EQUALS(u.getUser(), "");
        TS_ASSERT_EQUALS(u.getPassword(), "");
        TS_ASSERT_EQUALS(u.getHost(), "");
        TS_ASSERT_EQUALS(u.getPort(), "");
        TS_ASSERT_EQUALS(u.getPath(), "user@dom.ain");
        TS_ASSERT_EQUALS(u.getFragment(), "");
        TS_ASSERT(!u.isNull());
        TS_ASSERT( u.isValid());
        TS_ASSERT(!u.isValidHost());
        TS_ASSERT( u.isValidId());
        TS_ASSERT_EQUALS(u.toString(), "mailto:user@dom.ain");
    }
    {
        afl::net::Url u;
        TS_ASSERT(u.parse("data:image/gif;base64,R0lGODlhFgAWAID/AMDAwAAAACH5BAEAAAAALAAAAAAWABYAQAInhI+pa+H9mJy0LhdgtrxzDG5WGFVk6aXqyk6Y9kXvKKNuLbb6zgMFADs="));
        TS_ASSERT_EQUALS(u.getScheme(), "data");
        TS_ASSERT_EQUALS(u.getUser(), "");
        TS_ASSERT_EQUALS(u.getPassword(), "");
        TS_ASSERT_EQUALS(u.getHost(), "");
        TS_ASSERT_EQUALS(u.getPort(), "");
        TS_ASSERT_EQUALS(u.getPath().substr(0,20), "image/gif;base64,R0l");
        TS_ASSERT_EQUALS(u.getFragment(), "");
    }
    {
        afl::net::Url u;
        // TS_ASSERT(!u.parse(":")); Not invalid; not recognized as a null scheme
        // TS_ASSERT(!u.parse("foo:")); Not invalid.
        TS_ASSERT(!u.parse("http://[::1]?bar"));

        // Syntax error examples
        TS_ASSERT(!u.parse("foo://:@"));
        TS_ASSERT(!u.parse("foo://["));
        TS_ASSERT(!u.parse("foo://[::1]bla"));
        TS_ASSERT(!u.parse("foo://host:/"));
        TS_ASSERT(!u.parse("foo://host:123bla"));
    }
}

/** Test URL merging. */
void
TestNetUrl::testMerge()
{
    {
        afl::net::Url u, u2;
        TS_ASSERT(u.parse("http://foo/bar"));
        TS_ASSERT(u2.parse("#baz"));
        u2.mergeFrom(u);
        TS_ASSERT_EQUALS(u2.getScheme(), "http");
        TS_ASSERT_EQUALS(u2.getUser(), "");
        TS_ASSERT_EQUALS(u2.getPassword(), "");
        TS_ASSERT_EQUALS(u2.getHost(), "foo");
        TS_ASSERT_EQUALS(u2.getPort(), "");
        TS_ASSERT_EQUALS(u2.getPath(), "/bar");
        TS_ASSERT_EQUALS(u2.getFragment(), "#baz");
    }
    {
        afl::net::Url u, u2;
        TS_ASSERT(u.parse("http://foo/bar"));
        TS_ASSERT(u2.parse("baz"));
        u2.mergeFrom(u);
        TS_ASSERT_EQUALS(u2.getScheme(), "http");
        TS_ASSERT_EQUALS(u2.getUser(), "");
        TS_ASSERT_EQUALS(u2.getPassword(), "");
        TS_ASSERT_EQUALS(u2.getHost(), "foo");
        TS_ASSERT_EQUALS(u2.getPort(), "");
        TS_ASSERT_EQUALS(u2.getPath(), "/baz");
        TS_ASSERT_EQUALS(u2.getFragment(), "");
    }
    {
        afl::net::Url u, u2;
        TS_ASSERT(u.parse("http://foo/bar/"));
        TS_ASSERT(u2.parse("baz"));
        u2.mergeFrom(u);
        TS_ASSERT_EQUALS(u2.getScheme(), "http");
        TS_ASSERT_EQUALS(u2.getUser(), "");
        TS_ASSERT_EQUALS(u2.getPassword(), "");
        TS_ASSERT_EQUALS(u2.getHost(), "foo");
        TS_ASSERT_EQUALS(u2.getPort(), "");
        TS_ASSERT_EQUALS(u2.getPath(), "/bar/baz");
        TS_ASSERT_EQUALS(u2.getFragment(), "");
    }
    {
        afl::net::Url u, u2;
        TS_ASSERT(u.parse("http://foo/bar/baz/narf#boo"));
        TS_ASSERT(u2.parse("../fred"));
        u2.mergeFrom(u);
        TS_ASSERT_EQUALS(u2.getScheme(), "http");
        TS_ASSERT_EQUALS(u2.getUser(), "");
        TS_ASSERT_EQUALS(u2.getPassword(), "");
        TS_ASSERT_EQUALS(u2.getHost(), "foo");
        TS_ASSERT_EQUALS(u2.getPort(), "");
        TS_ASSERT_EQUALS(u2.getPath(), "/bar/fred");
        TS_ASSERT_EQUALS(u2.getFragment(), "");
    }
    {
        afl::net::Url u, u2;
        TS_ASSERT(u.parse("http://foo/bar/baz/narf#boo"));
        TS_ASSERT(u2.parse(".."));
        u2.mergeFrom(u);
        TS_ASSERT_EQUALS(u2.getScheme(), "http");
        TS_ASSERT_EQUALS(u2.getUser(), "");
        TS_ASSERT_EQUALS(u2.getPassword(), "");
        TS_ASSERT_EQUALS(u2.getHost(), "foo");
        TS_ASSERT_EQUALS(u2.getPort(), "");
        TS_ASSERT_EQUALS(u2.getPath(), "/bar");
        TS_ASSERT_EQUALS(u2.getFragment(), "");
    }
    {
        afl::net::Url u, u2;
        TS_ASSERT(u.parse("http://foo/"));
        TS_ASSERT(u2.parse(".."));
        u2.mergeFrom(u);
        TS_ASSERT_EQUALS(u2.getScheme(), "http");
        TS_ASSERT_EQUALS(u2.getUser(), "");
        TS_ASSERT_EQUALS(u2.getPassword(), "");
        TS_ASSERT_EQUALS(u2.getHost(), "foo");
        TS_ASSERT_EQUALS(u2.getPort(), "");
        TS_ASSERT_EQUALS(u2.getPath(), "/");
        TS_ASSERT_EQUALS(u2.getFragment(), "");
    }
    {
        afl::net::Url u, u2;
        TS_ASSERT(u.parse("http://foo:77/"));
        TS_ASSERT(u2.parse("//bar#1"));
        u2.mergeFrom(u);
        TS_ASSERT_EQUALS(u2.getScheme(), "http");
        TS_ASSERT_EQUALS(u2.getUser(), "");
        TS_ASSERT_EQUALS(u2.getPassword(), "");
        TS_ASSERT_EQUALS(u2.getHost(), "bar");
        TS_ASSERT_EQUALS(u2.getPort(), "");
        TS_ASSERT_EQUALS(u2.getPath(), "/");
        TS_ASSERT_EQUALS(u2.getFragment(), "#1");
    }
}

void
TestNetUrl::testSet()
{
    afl::net::Url u;
    TS_ASSERT(u.parse(""));
    TS_ASSERT_EQUALS(u.getScheme(), "");
    TS_ASSERT_EQUALS(u.getUser(), "");
    TS_ASSERT_EQUALS(u.getPassword(), "");
    TS_ASSERT_EQUALS(u.getHost(), "");
    TS_ASSERT_EQUALS(u.getPort(), "");
    TS_ASSERT_EQUALS(u.getPath(), "");
    TS_ASSERT_EQUALS(u.getFragment(), "");
    TS_ASSERT(u.isNull());
    TS_ASSERT(!u.isValid());
    TS_ASSERT(!u.isValidHost());
    TS_ASSERT(!u.isValidId());
    TS_ASSERT_EQUALS(u.toString(), "");

    u.setHost("host");
    TS_ASSERT_EQUALS(u.getHost(), "host");

    u.setPort("80");
    TS_ASSERT_EQUALS(u.getPort(), "80");

    u.setPassword("s3cr3t");
    TS_ASSERT_EQUALS(u.getPassword(), "s3cr3t");

    u.setUser("root");
    TS_ASSERT_EQUALS(u.getUser(), "root");

    u.setScheme("http");
    TS_ASSERT_EQUALS(u.getScheme(), "http");

    u.setPath("/login");
    TS_ASSERT_EQUALS(u.getPath(), "/login");

    u.setFragment("#top");
    TS_ASSERT_EQUALS(u.getFragment(), "#top");

    TS_ASSERT_EQUALS(u.toString(), "http://root:s3cr3t@host:80/login#top");
}

/** Test afl::net::matchPath(). */
void
TestNetUrl::testMatchPath()
{
    using afl::net::matchPath;

    String_t sub;
    TS_ASSERT(matchPath("/dir/sub", "", sub));
    TS_ASSERT_EQUALS(sub, "/dir/sub");
    TS_ASSERT(matchPath("/dir/sub", "/dir", sub));
    TS_ASSERT_EQUALS(sub, "/sub");
    TS_ASSERT(matchPath("/dir/sub", "/dir/sub", sub));
    TS_ASSERT_EQUALS(sub, "");
    TS_ASSERT(!matchPath("/dir/sub", "/", sub));
    TS_ASSERT(!matchPath("/dir/sub", "/di", sub));
    TS_ASSERT(!matchPath("/dir/sub", "/dir/sub/sub", sub));

    TS_ASSERT(matchPath("/dir/sub?q", "", sub));
    TS_ASSERT_EQUALS(sub, "/dir/sub?q");
    TS_ASSERT(matchPath("/dir/sub?q", "/dir", sub));
    TS_ASSERT_EQUALS(sub, "/sub?q");
    TS_ASSERT(matchPath("/dir/sub?q", "/dir/sub", sub));
    TS_ASSERT_EQUALS(sub, "?q");
    TS_ASSERT(!matchPath("/dir/sub?q", "/", sub));
    TS_ASSERT(!matchPath("/dir/sub?q", "/di", sub));
    TS_ASSERT(!matchPath("/dir/sub?q", "/dir/sub/sub", sub));
}

/** Test afl::net::matchArguments(). */
void
TestNetUrl::testMatchArgs()
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
    TS_ASSERT_EQUALS(thc.m_result, "");

    String_t path = "/foo";
    afl::net::matchArguments(path, thc);
    TS_ASSERT_EQUALS(path, "/foo");
    TS_ASSERT_EQUALS(thc.m_result, "");

    thc.m_result = "";
    path = "/foo?";
    afl::net::matchArguments(path, thc);
    TS_ASSERT_EQUALS(path, "/foo");
    TS_ASSERT_EQUALS(thc.m_result, "");

    thc.m_result = "";
    path = "/foo?x";
    afl::net::matchArguments(path, thc);
    TS_ASSERT_EQUALS(path, "/foo");
    TS_ASSERT_EQUALS(thc.m_result, "<<x>>(())");

    thc.m_result = "";
    path = "/foo?x&y&";
    afl::net::matchArguments(path, thc);
    TS_ASSERT_EQUALS(path, "/foo");
    TS_ASSERT_EQUALS(thc.m_result, "<<x>>(())<<y>>(())");

    thc.m_result = "";
    path = "/foo?x=y";
    afl::net::matchArguments(path, thc);
    TS_ASSERT_EQUALS(path, "/foo");
    TS_ASSERT_EQUALS(thc.m_result, "<<x>>((y))");

    thc.m_result = "";
    path = "/foo?x=y&z";
    afl::net::matchArguments(path, thc);
    TS_ASSERT_EQUALS(path, "/foo");
    TS_ASSERT_EQUALS(thc.m_result, "<<x>>((y))<<z>>(())");

    thc.m_result = "";
    path = "/foo?x=a+b+c";
    afl::net::matchArguments(path, thc);
    TS_ASSERT_EQUALS(path, "/foo");
    TS_ASSERT_EQUALS(thc.m_result, "<<x>>((a b c))");

    thc.m_result = "";
    path = "/foo?=a";
    afl::net::matchArguments(path, thc);
    TS_ASSERT_EQUALS(path, "/foo");
    TS_ASSERT_EQUALS(thc.m_result, "<<>>((a))");

    thc.m_result = "";
    path = "/foo?a=%3D%25";
    afl::net::matchArguments(path, thc);
    TS_ASSERT_EQUALS(path, "/foo");
    TS_ASSERT_EQUALS(thc.m_result, "<<a>>((=%))");

    thc.m_result = "";
    path = "/foo?%3D=%25";
    afl::net::matchArguments(path, thc);
    TS_ASSERT_EQUALS(path, "/foo");
    TS_ASSERT_EQUALS(thc.m_result, "<<=>>((%))");

    thc.m_result = "";
    path = "/foo?p=/foo";
    afl::net::matchArguments(path, thc);
    TS_ASSERT_EQUALS(path, "/foo");
    TS_ASSERT_EQUALS(thc.m_result, "<<p>>((/foo))");

    thc.m_result = "";
    path = "/foo?p=/foo?p=/foo";
    afl::net::matchArguments(path, thc);
    TS_ASSERT_EQUALS(path, "/foo");
    TS_ASSERT_EQUALS(thc.m_result, "<<p>>((/foo?p=/foo))");

    thc.m_result = "";
    path = "/foo?p=/foo?p=/foo&a=1";
    afl::net::matchArguments(path, thc);
    TS_ASSERT_EQUALS(path, "/foo");
    TS_ASSERT_EQUALS(thc.m_result, "<<p>>((/foo?p=/foo))<<a>>((1))");
}
