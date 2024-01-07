/**
  *  \file test/afl/net/http/requesttest.cpp
  *  \brief Test for afl::net::http::Request
  */

#include "afl/net/http/request.hpp"

#include "afl/string/string.hpp"
#include "afl/test/testrunner.hpp"

using afl::net::http::Request;
using afl::base::ConstBytes_t;

// HTTP/0.9 request
AFL_TEST("afl.net.http.Request:http-0.9:standard", a)
{
    Request req;
    ConstBytes_t bytes(afl::string::toBytes("GET /foo\r\n"));
    a.check("data accepted", req.handleData(bytes));
    a.check("data complete", bytes.empty());
    a.checkEqual("getMethod", req.getMethod(), "GET");
    a.checkEqual("getPath", req.getPath(), "/foo");
    a.checkEqual("getVersion", req.getVersion(), "HTTP/0.9");
    a.check("isResponseHeaderRequested", !req.isResponseHeaderRequested());
    a.check("isResponseBodyRequested", req.isResponseBodyRequested());
    a.check("hasErrors", !req.hasErrors());
}

// HTTP/0.9 request with more spaces
AFL_TEST("afl.net.http.Request:http-0.9:space", a)
{
    Request req;
    ConstBytes_t bytes(afl::string::toBytes("GET     /foo  \r\n"));
    a.check("data accepted", req.handleData(bytes));
    a.check("data complete", bytes.empty());
    a.checkEqual("getMethod", req.getMethod(), "GET");
    a.checkEqual("getPath", req.getPath(), "/foo");
    a.checkEqual("getVersion", req.getVersion(), "HTTP/0.9");
    a.check("isResponseHeaderRequested", !req.isResponseHeaderRequested());
    a.check("isResponseBodyRequested", req.isResponseBodyRequested());
    a.check("hasErrors", !req.hasErrors());
}

// Fragmented HTTP/0.9 request
AFL_TEST("afl.net.http.Request:http-0.9:fragment", a)
{
    Request req;
    ConstBytes_t bytes(afl::string::toBytes("GET /fo"));
    a.check("fragment not complete", !req.handleData(bytes));
    a.check("data complete", bytes.empty());
    bytes = afl::string::toBytes("o\r\nxy");
    a.check("data accepted", req.handleData(bytes));
    a.checkEqual("", bytes.size(), 2U);                    // "xy" remains
    a.checkEqual("getMethod", req.getMethod(), "GET");
    a.checkEqual("getPath", req.getPath(), "/foo");
    a.checkEqual("getVersion", req.getVersion(), "HTTP/0.9");
    a.check("isResponseHeaderRequested", !req.isResponseHeaderRequested());
    a.check("isResponseBodyRequested", req.isResponseBodyRequested());
    a.check("hasErrors", !req.hasErrors());
}

// HTTP/1.0 request
AFL_TEST("afl.net.http.Request:http-1.0", a)
{
    Request req;
    ConstBytes_t bytes(afl::string::toBytes("GET / HTTP/1.0\r\nContent-Type: text/json\r\nContent-Length: 10\r\n\r\n"));
    a.check("data accepted", req.handleData(bytes));
    a.check("data complete", bytes.empty());
    a.checkEqual("getMethod", req.getMethod(), "GET");
    a.checkEqual("getPath", req.getPath(), "/");
    a.checkEqual("getVersion", req.getVersion(), "HTTP/1.0");
    a.check("isResponseHeaderRequested", req.isResponseHeaderRequested());
    a.check("isResponseBodyRequested", req.isResponseBodyRequested());
    a.check("isKeepalive", !req.isKeepalive());
    a.check("hasErrors", !req.hasErrors());

    const afl::net::HeaderField* hf = req.headers().get("content-TYPE");  // case insensitive!
    a.checkNonNull("header get", hf);
    a.checkEqual("header value", hf->getValue(), "text/json");
}

// HTTP/1.0 request with keepalive
AFL_TEST("afl.net.http.Request:http-1.0:keepalive", a)
{
    Request req;
    ConstBytes_t bytes(afl::string::toBytes("GET / HTTP/1.0\r\nConnection: keepalive\r\n\r\n"));
    a.check("data accepted", req.handleData(bytes));
    a.check("data complete", bytes.empty());
    a.checkEqual("getMethod", req.getMethod(), "GET");
    a.checkEqual("getPath", req.getPath(), "/");
    a.checkEqual("getVersion", req.getVersion(), "HTTP/1.0");
    a.check("isResponseHeaderRequested", req.isResponseHeaderRequested());
    a.check("isResponseBodyRequested", req.isResponseBodyRequested());
    a.check("isKeepalive", req.isKeepalive());
    a.check("hasErrors", !req.hasErrors());
}

// HTTP/1.1 request
AFL_TEST("afl.net.http.Request:http-1.1", a)
{
    Request req;
    ConstBytes_t bytes(afl::string::toBytes("GET / HTTP/1.1\r\n\r\n"));
    a.check("data accepted", req.handleData(bytes));
    a.check("data complete", bytes.empty());
    a.checkEqual("getMethod", req.getMethod(), "GET");
    a.checkEqual("getPath", req.getPath(), "/");
    a.checkEqual("getVersion", req.getVersion(), "HTTP/1.1");
    a.check("isResponseHeaderRequested", req.isResponseHeaderRequested());
    a.check("isResponseBodyRequested", req.isResponseBodyRequested());
    a.check("isKeepalive", req.isKeepalive());
    a.check("hasErrors", !req.hasErrors());
}

// HTTP/1.0 request without keepalive
AFL_TEST("afl.net.http.Request:http-1.1:no-keepalive", a)
{
    Request req;
    ConstBytes_t bytes(afl::string::toBytes("GET / HTTP/1.1\r\nConnection: close\r\n\r\n"));
    a.check("data accepted", req.handleData(bytes));
    a.check("data complete", bytes.empty());
    a.checkEqual("getMethod", req.getMethod(), "GET");
    a.checkEqual("getPath", req.getPath(), "/");
    a.checkEqual("getVersion", req.getVersion(), "HTTP/1.1");
    a.check("isResponseHeaderRequested", req.isResponseHeaderRequested());
    a.check("isResponseBodyRequested", req.isResponseBodyRequested());
    a.check("isKeepalive", !req.isKeepalive());
    a.check("hasErrors", !req.hasErrors());
}

// Error
AFL_TEST("afl.net.http.Request:error", a)
{
    Request req;
    ConstBytes_t bytes(afl::string::toBytes("\r\n"));
    a.check("data accepted", req.handleData(bytes));
    a.check("hasErrors", req.hasErrors());
}

AFL_TEST("afl.net.http.Request:matchPath", a)
{
    Request req;
    String_t sub;
    ConstBytes_t bytes(afl::string::toBytes("GET /dir/sub HTTP/1.0\r\n\r\n"));
    a.check("data accepted", req.handleData(bytes));
    a.checkEqual("getPath", req.getPath(), "/dir/sub");
    a.check("matchPath 1", req.matchPath("", sub));
    a.checkEqual("sub 1", sub, "/dir/sub");
    a.check("matchPath 2", req.matchPath("/dir", sub));
    a.checkEqual("sub 2", sub, "/sub");
    a.check("matchPath 3", req.matchPath("/dir/sub", sub));
    a.checkEqual("sub 3", sub, "");
    a.check("matchPath 4", !req.matchPath("/", sub));
    a.check("matchPath 5", !req.matchPath("/di", sub));
    a.check("matchPath 6", !req.matchPath("/dir/sub/sub", sub));
}

AFL_TEST("afl.net.http.Request:matchPath:2", a)
{
    Request req;
    String_t sub;
    ConstBytes_t bytes(afl::string::toBytes("GET /dir/sub?q HTTP/1.0\r\n\r\n"));
    a.check("data accepted", req.handleData(bytes));
    a.checkEqual("getPath", req.getPath(), "/dir/sub?q");
    a.check("matchPath 1", req.matchPath("", sub));
    a.checkEqual("sub 1", sub, "/dir/sub?q");
    a.check("matchPath 2", req.matchPath("/dir", sub));
    a.checkEqual("sub 2", sub, "/sub?q");
    a.check("matchPath 3", req.matchPath("/dir/sub", sub));
    a.checkEqual("sub 3", sub, "?q");
    a.check("matchPath 4", !req.matchPath("/", sub));
    a.check("matchPath 5", !req.matchPath("/di", sub));
    a.check("matchPath 6", !req.matchPath("/dir/sub/sub", sub));
}
