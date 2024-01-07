/**
  *  \file test/afl/net/http/clientresponsetest.cpp
  *  \brief Test for afl::net::http::ClientResponse
  */

#include "afl/net/http/clientresponse.hpp"
#include "afl/test/testrunner.hpp"

/*
 *  Good responses
 */

// Simple case
AFL_TEST("afl.net.http.ClientResponse:good:http-1.0", a)
{
    afl::net::http::ClientResponse resp(false);
    afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.0 200 OK\r\n"
                                                        "Content-Length: 3\r\n"
                                                        "\r\n"
                                                        "foo").toBytes());
    bool ok = resp.handleData(bytes);
    a.check("ok", ok);
    a.check("hasErrors",             !resp.hasErrors());
    a.check("hasBody",                resp.hasBody());
    a.checkEqual("getStatusCode",     resp.getStatusCode(), 200U);
    a.checkEqual("getStatusPhrase",   resp.getStatusPhrase(), "OK");
    a.checkEqual("getVersion",        resp.getVersion(), "HTTP/1.0");
    a.checkEqual("getResponseLimitKind", resp.getResponseLimitKind(), resp.ByteLimit);
    a.checkEqual("getResponseEncoding", resp.getResponseEncoding(), resp.IdentityEncoding);
    a.checkEqual("getResponseLength", resp.getResponseLength(), 3U);
    a.checkEqual("getResponseOffset", resp.getResponseOffset(), 0U);
    a.checkEqual("getTotalLength",    resp.getTotalLength(), 3U);
    a.check("isKeepalive",           !resp.isKeepalive());
}

// Same thing, HTTP/1.1 (=keepalive)
AFL_TEST("afl.net.http.ClientResponse:good:http-1.1", a)
{
    afl::net::http::ClientResponse resp(false);
    afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.1 200 OK\r\n"
                                                        "Content-Length: 3\r\n"
                                                        "\r\n"
                                                        "foo").toBytes());
    bool ok = resp.handleData(bytes);
    a.check("ok", ok);
    a.check("hasErrors",             !resp.hasErrors());
    a.check("hasBody",                resp.hasBody());
    a.checkEqual("getStatusCode",     resp.getStatusCode(), 200U);
    a.checkEqual("getStatusPhrase",   resp.getStatusPhrase(), "OK");
    a.checkEqual("getVersion",        resp.getVersion(), "HTTP/1.1");
    a.checkEqual("getResponseLimitKind", resp.getResponseLimitKind(), resp.ByteLimit);
    a.checkEqual("getResponseEncoding", resp.getResponseEncoding(), resp.IdentityEncoding);
    a.checkEqual("getResponseLength", resp.getResponseLength(), 3U);
    a.checkEqual("getResponseOffset", resp.getResponseOffset(), 0U);
    a.checkEqual("getTotalLength",    resp.getTotalLength(), 3U);
    a.check("isKeepalive",            resp.isKeepalive());
}

// Same thing, HEAD (=no body)
AFL_TEST("afl.net.http.ClientResponse:good:http-1.1:head", a)
{
    afl::net::http::ClientResponse resp(true);
    afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.1 200 OK\r\n"
                                                        "Content-Length: 3\r\n"
                                                        "\r\n"
                                                        "foo").toBytes());
    bool ok = resp.handleData(bytes);
    a.check("ok", ok);
    a.check("hasErrors",             !resp.hasErrors());
    a.check("hasBody",               !resp.hasBody());
    a.checkEqual("getStatusCode",     resp.getStatusCode(), 200U);
    a.checkEqual("getStatusPhrase",   resp.getStatusPhrase(), "OK");
    a.checkEqual("getVersion",        resp.getVersion(), "HTTP/1.1");
    a.checkEqual("getResponseLimitKind", resp.getResponseLimitKind(), resp.ByteLimit);
    a.checkEqual("getResponseEncoding", resp.getResponseEncoding(), resp.IdentityEncoding);
    a.checkEqual("getResponseLength", resp.getResponseLength(), 0U);
    a.checkEqual("getResponseOffset", resp.getResponseOffset(), 0U);
    a.checkEqual("getTotalLength",    resp.getTotalLength(), 3U);
    a.check("isKeepalive",            resp.isKeepalive());
}

// Byte range
AFL_TEST("afl.net.http.ClientResponse:good:content-range", a)
{
    afl::net::http::ClientResponse resp(false);
    afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.1 206 Partial\r\n"
                                                        "Content-Length: 100\r\n"
                                                        "Content-Range: bytes 10-15/100\r\n"
                                                        "\r\n"
                                                        "foobar").toBytes());
    bool ok = resp.handleData(bytes);
    a.check("ok", ok);
    a.check("hasErrors",             !resp.hasErrors());
    a.check("hasBody",                resp.hasBody());
    a.checkEqual("getStatusCode",     resp.getStatusCode(), 206U);
    a.checkEqual("getStatusPhrase",   resp.getStatusPhrase(), "Partial");
    a.checkEqual("getVersion",        resp.getVersion(), "HTTP/1.1");
    a.checkEqual("getResponseLimitKind", resp.getResponseLimitKind(), resp.ByteLimit);
    a.checkEqual("getResponseEncoding", resp.getResponseEncoding(), resp.IdentityEncoding);
    a.checkEqual("getResponseLength", resp.getResponseLength(), 6U);
    a.checkEqual("getResponseOffset", resp.getResponseOffset(), 10U);
    a.checkEqual("getTotalLength",    resp.getTotalLength(), 100U);
    a.check("isKeepalive",            resp.isKeepalive());
}

// Chunked
AFL_TEST("afl.net.http.ClientResponse:good:chunked", a)
{
    afl::net::http::ClientResponse resp(false);
    afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.1 200 OK\r\n"
                                                        "Transfer-Encoding: chunked\r\n"
                                                        "\r\n"
                                                        "foobar").toBytes());
    bool ok = resp.handleData(bytes);
    a.check("ok", ok);
    a.check("hasErrors",             !resp.hasErrors());
    a.check("hasBody",                resp.hasBody());
    a.checkEqual("getStatusCode",     resp.getStatusCode(), 200U);
    a.checkEqual("getStatusPhrase",   resp.getStatusPhrase(), "OK");
    a.checkEqual("getVersion",        resp.getVersion(), "HTTP/1.1");
    a.checkEqual("getResponseLimitKind", resp.getResponseLimitKind(), resp.ChunkLimit);
    a.checkEqual("getResponseEncoding", resp.getResponseEncoding(), resp.IdentityEncoding);
    a.checkEqual("getResponseLength", resp.getResponseLength(), 0U);
    a.checkEqual("getResponseOffset", resp.getResponseOffset(), 0U);
    a.checkEqual("getTotalLength",    resp.getTotalLength(), 0U);
    a.check("isKeepalive",            resp.isKeepalive());
}

// No delimiter
AFL_TEST("afl.net.http.ClientResponse:good:undelimited", a)
{
    afl::net::http::ClientResponse resp(false);
    afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.1 200 OK\r\n"
                                                        "\r\n"
                                                        "foobar").toBytes());
    bool ok = resp.handleData(bytes);
    a.check("ok", ok);
    a.check("hasErrors",             !resp.hasErrors());
    a.check("hasBody",                resp.hasBody());
    a.checkEqual("getStatusCode",     resp.getStatusCode(), 200U);
    a.checkEqual("getStatusPhrase",   resp.getStatusPhrase(), "OK");
    a.checkEqual("getVersion",        resp.getVersion(), "HTTP/1.1");
    a.checkEqual("getResponseLimitKind", resp.getResponseLimitKind(), resp.StreamLimit);
    a.checkEqual("getResponseEncoding", resp.getResponseEncoding(), resp.IdentityEncoding);
    a.checkEqual("getResponseLength", resp.getResponseLength(), 0U);
    a.checkEqual("getResponseOffset", resp.getResponseOffset(), 0U);
    a.checkEqual("getTotalLength",    resp.getTotalLength(), 0U);
    a.check("isKeepalive",           !resp.isKeepalive());
}

/*
 *   Bad responses
 */

// Bad intro
AFL_TEST("afl.net.http.ClientResponse:bad:intro", a)
{
    afl::net::http::ClientResponse resp(false);
    afl::base::ConstBytes_t bytes(afl::string::toBytes("Hi mom!\r\n"));
    bool ok = resp.handleData(bytes);
    a.check("ok", ok);
    a.check("hasErrors", resp.hasErrors());
}

// Bad intro
AFL_TEST("afl.net.http.ClientResponse:bad:intro2", a)
{
    afl::net::http::ClientResponse resp(false);
    afl::base::ConstBytes_t bytes(afl::string::toBytes("lol\r\n"));
    bool ok = resp.handleData(bytes);
    a.check("ok", ok);
    a.check("hasErrors", resp.hasErrors());
}

// Overlong version
AFL_TEST("afl.net.http.ClientResponse:bad:version", a)
{
    afl::net::http::ClientResponse resp(false);
    afl::base::ConstBytes_t bytes(afl::string::toBytes("HTTP/3.1415926535897932384626433832795028841968 200 OK\r\n"));
    bool ok = resp.handleData(bytes);
    a.check("ok", ok);
    a.check("hasErrors", resp.hasErrors());
}

// Bad code
AFL_TEST("afl.net.http.ClientResponse:bad:no-version", a)
{
    afl::net::http::ClientResponse resp(false);
    afl::base::ConstBytes_t bytes(afl::string::toBytes("200 OK\r\n"));
    bool ok = resp.handleData(bytes);
    a.check("ok", ok);
    a.check("hasErrors", resp.hasErrors());
}

// Bad header
AFL_TEST("afl.net.http.ClientResponse:bad:header", a)
{
    afl::net::http::ClientResponse resp(false);
    afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.0 200 OK\r\n"
                                                        " foo : bar\r\n\r\n").toBytes());
    bool ok = resp.handleData(bytes);
    a.check("ok", ok);
    a.check("hasErrors", resp.hasErrors());
}

// Partial (parse does not finish)
AFL_TEST("afl.net.http.ClientResponse:bad:incomplete", a)
{
    afl::net::http::ClientResponse resp(false);
    afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.0 200 OK\r\n"
                                                        "Content-Length: 3\r\n").toBytes());
    bool ok = resp.handleData(bytes);
    a.check("ok", !ok);
}

/*
 *  Header values
 */

// Basic
AFL_TEST("afl.net.http.ClientResponse:header:basic", a)
{
    afl::net::http::ClientResponse resp(false);
    afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.0 200 OK\r\n"
                                                        "Content-Type: text/plain\r\n"
                                                        "Server: whatever\r\n"
                                                        "Content-Length: 0\r\n\r\n").toBytes());
    bool ok = resp.handleData(bytes);
    a.check("ok", ok);

    const afl::net::HeaderField* f = resp.getResponseHeaders().get("content-type");
    a.checkNonNull("header exists 1", f);
    a.checkEqual("header name 1",  f->getName(), "Content-Type");
    a.checkEqual("header value 1", f->getValue(), "text/plain");

    f = resp.getResponseHeaders().get("server");
    a.checkNonNull("header exists 2", f);
    a.checkEqual("header name 2",  f->getName(), "Server");
    a.checkEqual("header value 2", f->getValue(), "whatever");
}

// Duplicate header
AFL_TEST("afl.net.http.ClientResponse:header:dup", a)
{
    afl::net::http::ClientResponse resp(false);
    afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.0 200 OK\r\n"
                                                        "Content-Type: text/plain\r\n"
                                                        "Content-Type: whatever\r\n"
                                                        "Content-Length: 0\r\n\r\n").toBytes());
    bool ok = resp.handleData(bytes);
    a.check("ok", ok);

    const afl::net::HeaderField* f = resp.getResponseHeaders().get("content-type");
    a.checkNonNull("header exists", f);
    a.checkEqual("header name", f->getName(), "Content-Type");
    a.checkEqual("header value", f->getValue(), "text/plain");

    f = resp.getResponseHeaders().get("server");
    a.checkNull("header does not exist", f);
}

// Cookie (example from RFC 6265)
AFL_TEST("afl.net.http.ClientResponse:header:cookie", a)
{
    afl::net::http::ClientResponse resp(false);
    afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.0 200 OK\r\n"
                                                        "Set-Cookie: SID=31d4d96e407aad42; Path=/; Secure; HttpOnly\r\n"
                                                        "Set-Cookie: lang=en-US; Path=/; Domain=example.com\r\n"
                                                        "Content-Type: text/plain\r\n"
                                                        "Content-Length: 0\r\n\r\n").toBytes());
    bool ok = resp.handleData(bytes);
    a.check("ok", ok);

    const afl::net::HeaderField* f = resp.getResponseHeaders().get("content-type");
    a.checkNonNull("header exists 1", f);
    a.checkEqual("header name 1",  f->getName(), "Content-Type");
    a.checkEqual("header value 1", f->getValue(), "text/plain");

    f = resp.getResponseCookies().get("sid");
    a.checkNonNull("header exists 2", f);
    a.checkEqual("header name 2",  f->getName(), "SID");
    a.checkEqual("header prim 2",  f->getPrimaryValue(), "31d4d96e407aad42");
    a.checkEqual("header value 2", f->getValue(), "31d4d96e407aad42; Path=/; Secure; HttpOnly");

    String_t tmp;
    a.check("secondary path", f->getSecondaryValue("path", tmp));
    a.checkEqual("path value", tmp, "/");
    a.check("secondary httponly", f->getSecondaryValue("httponly", tmp));
    a.checkEqual("httponly value", tmp, "");

    f = resp.getResponseCookies().get("lang");
    a.checkNonNull("header exists 3", f);
    a.checkEqual("header prim 3", f->getPrimaryValue(), "en-US");
}

/*
 *  Keepalive handling, HTTP 1.0
 */

// No content-length, so cannot be keepalive
AFL_TEST("afl.net.http.ClientResponse:keepalive:1.0:undelimited", a)
{
    afl::net::http::ClientResponse resp(false);
    afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.0 200 OK\r\n"
                                                        "\r\n").toBytes());
    bool ok = resp.handleData(bytes);
    a.check("ok", ok);
    a.check("isKeepalive", !resp.isKeepalive());
}

// Content-length given, but opt-in required for 1.0
AFL_TEST("afl.net.http.ClientResponse:keepalive:1.0:length", a)
{
    afl::net::http::ClientResponse resp(false);
    afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.0 200 OK\r\n"
                                                        "Content-Length: 3\r\n"
                                                        "\r\n").toBytes());
    bool ok = resp.handleData(bytes);
    a.check("ok", ok);
    a.check("isKeepalive", !resp.isKeepalive());
}

// Content-length given, but opt-in required for 1.0
AFL_TEST("afl.net.http.ClientResponse:keepalive:1.0:opt-in", a)
{
    afl::net::http::ClientResponse resp(false);
    afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.0 200 OK\r\n"
                                                        "Content-Length: 3\r\n"
                                                        "Connection: keep-alive\r\n"
                                                        "\r\n").toBytes());
    bool ok = resp.handleData(bytes);
    a.check("ok", ok);
    a.check("isKeepalive", resp.isKeepalive());
}

// Content-length given, bogus Connection header
AFL_TEST("afl.net.http.ClientResponse:keepalive:1.0:bad-value", a)
{
    afl::net::http::ClientResponse resp(false);
    afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.0 200 OK\r\n"
                                                        "Content-Length: 3\r\n"
                                                        "Connection: lol\r\n"
                                                        "\r\n").toBytes());
    bool ok = resp.handleData(bytes);
    a.check("ok", ok);
    a.check("isKeepalive", !resp.isKeepalive());
}

/*
 *  Keepalive handling, HTTP 1.1
 */

// No content-length, so cannot be keepalive
AFL_TEST("afl.net.http.ClientResponse:keepalive:1.1:undelimited", a)
{
    afl::net::http::ClientResponse resp(false);
    afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.1 200 OK\r\n"
                                                        "\r\n").toBytes());
    bool ok = resp.handleData(bytes);
    a.check("ok", ok);
    a.check("isKeepalive", !resp.isKeepalive());
}

// Content-length given, so ok
AFL_TEST("afl.net.http.ClientResponse:keepalive:1.1:length", a)
{
    afl::net::http::ClientResponse resp(false);
    afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.1 200 OK\r\n"
                                                        "Content-Length: 3\r\n"
                                                        "\r\n").toBytes());
    bool ok = resp.handleData(bytes);
    a.check("ok", ok);
    a.check("isKeepalive", resp.isKeepalive());
}

// Content-length given, but opt-out
AFL_TEST("afl.net.http.ClientResponse:keepalive:1.1:opt-out", a)
{
    afl::net::http::ClientResponse resp(false);
    afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.1 200 OK\r\n"
                                                        "Content-Length: 3\r\n"
                                                        "Connection: close\r\n"
                                                        "\r\n").toBytes());
    bool ok = resp.handleData(bytes);
    a.check("ok", ok);
    a.check("isKeepalive", !resp.isKeepalive());
}

// Case-insensitive
AFL_TEST("afl.net.http.ClientResponse:keepalive:1.1:case", a)
{
    afl::net::http::ClientResponse resp(false);
    afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.1 200 OK\r\n"
                                                        "CONTENT-LENGTH: 3\r\n"
                                                        "CONNECTION: CLOSE\r\n"
                                                        "\r\n").toBytes());
    bool ok = resp.handleData(bytes);
    a.check("ok", ok);
    a.check("isKeepalive", !resp.isKeepalive());
}

// Content-length given, bogus Connection header
AFL_TEST("afl.net.http.ClientResponse:keepalive:1.1:bad-value", a)
{
    afl::net::http::ClientResponse resp(false);
    afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.1 200 OK\r\n"
                                                        "Content-Length: 3\r\n"
                                                        "Connection: lol\r\n"
                                                        "\r\n").toBytes());
    bool ok = resp.handleData(bytes);
    a.check("ok", ok);
    a.check("isKeepalive", resp.isKeepalive());
}


/*
 *  Content-Encoding
 */

// Default
AFL_TEST("afl.net.http.ClientResponse:encoding:default", a)
{
    afl::net::http::ClientResponse resp(false);
    afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.1 200 OK\r\n"
                                                        "Content-Length: 3\r\n"
                                                        "\r\n").toBytes());
    bool ok = resp.handleData(bytes);
    a.check("ok", ok);
    a.checkEqual("getResponseEncoding", resp.getResponseEncoding(), afl::net::http::ClientResponse::IdentityEncoding);
}

// gzip
AFL_TEST("afl.net.http.ClientResponse:encoding:gzip", a)
{
    afl::net::http::ClientResponse resp(false);
    afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.1 200 OK\r\n"
                                                        "Content-Encoding: gzip\r\n"
                                                        "\r\n").toBytes());
    bool ok = resp.handleData(bytes);
    a.check("ok", ok);
    a.checkEqual("getResponseEncoding", resp.getResponseEncoding(), afl::net::http::ClientResponse::GzipEncoding);
}

// deflate
AFL_TEST("afl.net.http.ClientResponse:encoding:deflate", a)
{
    afl::net::http::ClientResponse resp(false);
    afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.1 200 OK\r\n"
                                                        "Content-Encoding: deflate\r\n"
                                                        "\r\n").toBytes());
    bool ok = resp.handleData(bytes);
    a.check("ok", ok);
    a.checkEqual("getResponseEncoding", resp.getResponseEncoding(), afl::net::http::ClientResponse::DeflateEncoding);
}

// case-insensitive!
AFL_TEST("afl.net.http.ClientResponse:encoding:case", a)
{
    afl::net::http::ClientResponse resp(false);
    afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.1 200 OK\r\n"
                                                        "CONTENT-ENCODING: DEFLATE\r\n"
                                                        "\r\n").toBytes());
    bool ok = resp.handleData(bytes);
    a.check("ok", ok);
    a.checkEqual("getResponseEncoding", resp.getResponseEncoding(), afl::net::http::ClientResponse::DeflateEncoding);
}

// unknown
AFL_TEST("afl.net.http.ClientResponse:encoding:unknown", a)
{
    afl::net::http::ClientResponse resp(false);
    afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.1 200 OK\r\n"
                                                        "Content-Encoding: 1337\r\n"
                                                        "\r\n").toBytes());
    bool ok = resp.handleData(bytes);
    a.check("ok", ok);
    a.checkEqual("getResponseEncoding", resp.getResponseEncoding(), afl::net::http::ClientResponse::UnknownEncoding);
}
