/**
  *  \file u/t_net_http_request.cpp
  *  \brief Test for afl::net::http::Request
  */

#include "afl/net/http/request.hpp"

#include "u/t_net_http.hpp"
#include "afl/string/string.hpp"

/** Assorted simple tests. */
void
TestNetHttpRequest::testIt()
{
    using afl::net::http::Request;
    using afl::base::ConstBytes_t;
    {
        // HTTP/0.9 request
        Request req;
        ConstBytes_t bytes(afl::string::toBytes("GET /foo\r\n"));
        TS_ASSERT(req.handleData(bytes));
        TS_ASSERT(bytes.empty());
        TS_ASSERT_EQUALS(req.getMethod(), "GET");
        TS_ASSERT_EQUALS(req.getPath(), "/foo");
        TS_ASSERT_EQUALS(req.getVersion(), "HTTP/0.9");
        TS_ASSERT(!req.isResponseHeaderRequested());
        TS_ASSERT(req.isResponseBodyRequested());
        TS_ASSERT(!req.hasErrors());
    }
    {
        // HTTP/0.9 request with more spaces
        Request req;
        ConstBytes_t bytes(afl::string::toBytes("GET     /foo  \r\n"));
        TS_ASSERT(req.handleData(bytes));
        TS_ASSERT(bytes.empty());
        TS_ASSERT_EQUALS(req.getMethod(), "GET");
        TS_ASSERT_EQUALS(req.getPath(), "/foo");
        TS_ASSERT_EQUALS(req.getVersion(), "HTTP/0.9");
        TS_ASSERT(!req.isResponseHeaderRequested());
        TS_ASSERT(req.isResponseBodyRequested());
        TS_ASSERT(!req.hasErrors());
    }
    {
        // Fragmented HTTP/0.9 request
        Request req;
        ConstBytes_t bytes(afl::string::toBytes("GET /fo"));
        TS_ASSERT(!req.handleData(bytes));
        TS_ASSERT(bytes.empty());
        bytes = afl::string::toBytes("o\r\nxy");
        TS_ASSERT(req.handleData(bytes));
        TS_ASSERT_EQUALS(bytes.size(), 2U);                    // "xy" remains
        TS_ASSERT_EQUALS(req.getMethod(), "GET");
        TS_ASSERT_EQUALS(req.getPath(), "/foo");
        TS_ASSERT_EQUALS(req.getVersion(), "HTTP/0.9");
        TS_ASSERT(!req.isResponseHeaderRequested());
        TS_ASSERT(req.isResponseBodyRequested());
        TS_ASSERT(!req.hasErrors());
    }
    {
        // HTTP/1.0 request
        Request req;
        ConstBytes_t bytes(afl::string::toBytes("GET / HTTP/1.0\r\nContent-Type: text/json\r\nContent-Length: 10\r\n\r\n"));
        TS_ASSERT(req.handleData(bytes));
        TS_ASSERT(bytes.empty());
        TS_ASSERT_EQUALS(req.getMethod(), "GET");
        TS_ASSERT_EQUALS(req.getPath(), "/");
        TS_ASSERT_EQUALS(req.getVersion(), "HTTP/1.0");
        TS_ASSERT(req.isResponseHeaderRequested());
        TS_ASSERT(req.isResponseBodyRequested());
        TS_ASSERT(!req.isKeepalive());
        TS_ASSERT(!req.hasErrors());

        const afl::net::HeaderField* hf = req.headers().get("content-TYPE");  // case insensitive!
        TS_ASSERT(hf != 0);
        TS_ASSERT_EQUALS(hf->getValue(), "text/json");
    }
    {
        // HTTP/1.0 request with keepalive
        Request req;
        ConstBytes_t bytes(afl::string::toBytes("GET / HTTP/1.0\r\nConnection: keepalive\r\n\r\n"));
        TS_ASSERT(req.handleData(bytes));
        TS_ASSERT(bytes.empty());
        TS_ASSERT_EQUALS(req.getMethod(), "GET");
        TS_ASSERT_EQUALS(req.getPath(), "/");
        TS_ASSERT_EQUALS(req.getVersion(), "HTTP/1.0");
        TS_ASSERT(req.isResponseHeaderRequested());
        TS_ASSERT(req.isResponseBodyRequested());
        TS_ASSERT(req.isKeepalive());
        TS_ASSERT(!req.hasErrors());
    }
    {
        // HTTP/1.1 request
        Request req;
        ConstBytes_t bytes(afl::string::toBytes("GET / HTTP/1.1\r\n\r\n"));
        TS_ASSERT(req.handleData(bytes));
        TS_ASSERT(bytes.empty());
        TS_ASSERT_EQUALS(req.getMethod(), "GET");
        TS_ASSERT_EQUALS(req.getPath(), "/");
        TS_ASSERT_EQUALS(req.getVersion(), "HTTP/1.1");
        TS_ASSERT(req.isResponseHeaderRequested());
        TS_ASSERT(req.isResponseBodyRequested());
        TS_ASSERT(req.isKeepalive());
        TS_ASSERT(!req.hasErrors());
    }
    {
        // HTTP/1.0 request without keepalive
        Request req;
        ConstBytes_t bytes(afl::string::toBytes("GET / HTTP/1.1\r\nConnection: close\r\n\r\n"));
        TS_ASSERT(req.handleData(bytes));
        TS_ASSERT(bytes.empty());
        TS_ASSERT_EQUALS(req.getMethod(), "GET");
        TS_ASSERT_EQUALS(req.getPath(), "/");
        TS_ASSERT_EQUALS(req.getVersion(), "HTTP/1.1");
        TS_ASSERT(req.isResponseHeaderRequested());
        TS_ASSERT(req.isResponseBodyRequested());
        TS_ASSERT(!req.isKeepalive());
        TS_ASSERT(!req.hasErrors());
    }
    {
        // Error
        Request req;
        ConstBytes_t bytes(afl::string::toBytes("\r\n"));
        TS_ASSERT(req.handleData(bytes));
        TS_ASSERT(req.hasErrors());
    }
}

/** Test matchPath(). */
void
TestNetHttpRequest::testPath()
{
    using afl::net::http::Request;
    using afl::base::ConstBytes_t;

    {
        Request req;
        String_t sub;
        ConstBytes_t bytes(afl::string::toBytes("GET /dir/sub HTTP/1.0\r\n\r\n"));
        TS_ASSERT(req.handleData(bytes));
        TS_ASSERT_EQUALS(req.getPath(), "/dir/sub");
        TS_ASSERT(req.matchPath("", sub));
        TS_ASSERT_EQUALS(sub, "/dir/sub");
        TS_ASSERT(req.matchPath("/dir", sub));
        TS_ASSERT_EQUALS(sub, "/sub");
        TS_ASSERT(req.matchPath("/dir/sub", sub));
        TS_ASSERT_EQUALS(sub, "");
        TS_ASSERT(!req.matchPath("/", sub));
        TS_ASSERT(!req.matchPath("/di", sub));
        TS_ASSERT(!req.matchPath("/dir/sub/sub", sub));
    }
    {
        Request req;
        String_t sub;
        ConstBytes_t bytes(afl::string::toBytes("GET /dir/sub?q HTTP/1.0\r\n\r\n"));
        TS_ASSERT(req.handleData(bytes));
        TS_ASSERT_EQUALS(req.getPath(), "/dir/sub?q");
        TS_ASSERT(req.matchPath("", sub));
        TS_ASSERT_EQUALS(sub, "/dir/sub?q");
        TS_ASSERT(req.matchPath("/dir", sub));
        TS_ASSERT_EQUALS(sub, "/sub?q");
        TS_ASSERT(req.matchPath("/dir/sub", sub));
        TS_ASSERT_EQUALS(sub, "?q");
        TS_ASSERT(!req.matchPath("/", sub));
        TS_ASSERT(!req.matchPath("/di", sub));
        TS_ASSERT(!req.matchPath("/dir/sub/sub", sub));
    }
}
