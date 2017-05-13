/**
  *  \file u/t_net_http_clientresponse.cpp
  *  \brief Test for afl::net::http::ClientResponse
  */

#include "afl/net/http/clientresponse.hpp"

#include "u/t_net_http.hpp"

/** Test good responses. */
void
TestNetHttpClientResponse::testOK()
{
    {
        // Simple case
        afl::net::http::ClientResponse resp(false);
        afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.0 200 OK\r\n"
                                                            "Content-Length: 3\r\n"
                                                            "\r\n"
                                                            "foo").toBytes());
        bool ok = resp.handleData(bytes);
        TS_ASSERT(ok);
        TS_ASSERT(!resp.hasErrors());
        TS_ASSERT(resp.hasBody());
        TS_ASSERT_EQUALS(resp.getStatusCode(), 200U);
        TS_ASSERT_EQUALS(resp.getStatusPhrase(), "OK");
        TS_ASSERT_EQUALS(resp.getVersion(), "HTTP/1.0");
        TS_ASSERT_EQUALS(resp.getResponseLimitKind(), resp.ByteLimit);
        TS_ASSERT_EQUALS(resp.getResponseEncoding(), resp.IdentityEncoding);
        TS_ASSERT_EQUALS(resp.getResponseLength(), 3U);
        TS_ASSERT_EQUALS(resp.getResponseOffset(), 0U);
        TS_ASSERT_EQUALS(resp.getTotalLength(), 3U);
        TS_ASSERT(!resp.isKeepalive());
    }

    {
        // Same thing, HTTP/1.1 (=keepalive)
        afl::net::http::ClientResponse resp(false);
        afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.1 200 OK\r\n"
                                                            "Content-Length: 3\r\n"
                                                            "\r\n"
                                                            "foo").toBytes());
        bool ok = resp.handleData(bytes);
        TS_ASSERT(ok);
        TS_ASSERT(!resp.hasErrors());
        TS_ASSERT(resp.hasBody());
        TS_ASSERT_EQUALS(resp.getStatusCode(), 200U);
        TS_ASSERT_EQUALS(resp.getStatusPhrase(), "OK");
        TS_ASSERT_EQUALS(resp.getVersion(), "HTTP/1.1");
        TS_ASSERT_EQUALS(resp.getResponseLimitKind(), resp.ByteLimit);
        TS_ASSERT_EQUALS(resp.getResponseEncoding(), resp.IdentityEncoding);
        TS_ASSERT_EQUALS(resp.getResponseLength(), 3U);
        TS_ASSERT_EQUALS(resp.getResponseOffset(), 0U);
        TS_ASSERT_EQUALS(resp.getTotalLength(), 3U);
        TS_ASSERT(resp.isKeepalive());
    }

    {
        // Same thing, HEAD (=no body)
        afl::net::http::ClientResponse resp(true);
        afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.1 200 OK\r\n"
                                                            "Content-Length: 3\r\n"
                                                            "\r\n"
                                                            "foo").toBytes());
        bool ok = resp.handleData(bytes);
        TS_ASSERT(ok);
        TS_ASSERT(!resp.hasErrors());
        TS_ASSERT(!resp.hasBody());
        TS_ASSERT_EQUALS(resp.getStatusCode(), 200U);
        TS_ASSERT_EQUALS(resp.getStatusPhrase(), "OK");
        TS_ASSERT_EQUALS(resp.getVersion(), "HTTP/1.1");
        TS_ASSERT_EQUALS(resp.getResponseLimitKind(), resp.ByteLimit);
        TS_ASSERT_EQUALS(resp.getResponseEncoding(), resp.IdentityEncoding);
        TS_ASSERT_EQUALS(resp.getResponseLength(), 0U);
        TS_ASSERT_EQUALS(resp.getResponseOffset(), 0U);
        TS_ASSERT_EQUALS(resp.getTotalLength(), 3U);
        TS_ASSERT(resp.isKeepalive());
    }

    {
        // Byte range
        afl::net::http::ClientResponse resp(false);
        afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.1 206 Partial\r\n"
                                                            "Content-Length: 100\r\n"
                                                            "Content-Range: bytes 10-15/100\r\n"
                                                            "\r\n"
                                                            "foobar").toBytes());
        bool ok = resp.handleData(bytes);
        TS_ASSERT(ok);
        TS_ASSERT(!resp.hasErrors());
        TS_ASSERT(resp.hasBody());
        TS_ASSERT_EQUALS(resp.getStatusCode(), 206U);
        TS_ASSERT_EQUALS(resp.getStatusPhrase(), "Partial");
        TS_ASSERT_EQUALS(resp.getVersion(), "HTTP/1.1");
        TS_ASSERT_EQUALS(resp.getResponseLimitKind(), resp.ByteLimit);
        TS_ASSERT_EQUALS(resp.getResponseEncoding(), resp.IdentityEncoding);
        TS_ASSERT_EQUALS(resp.getResponseLength(), 6U);
        TS_ASSERT_EQUALS(resp.getResponseOffset(), 10U);
        TS_ASSERT_EQUALS(resp.getTotalLength(), 100U);
        TS_ASSERT(resp.isKeepalive());
    }

    {
        // Chunked
        afl::net::http::ClientResponse resp(false);
        afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.1 200 OK\r\n"
                                                            "Transfer-Encoding: chunked\r\n"
                                                            "\r\n"
                                                            "foobar").toBytes());
        bool ok = resp.handleData(bytes);
        TS_ASSERT(ok);
        TS_ASSERT(!resp.hasErrors());
        TS_ASSERT(resp.hasBody());
        TS_ASSERT_EQUALS(resp.getStatusCode(), 200U);
        TS_ASSERT_EQUALS(resp.getStatusPhrase(), "OK");
        TS_ASSERT_EQUALS(resp.getVersion(), "HTTP/1.1");
        TS_ASSERT_EQUALS(resp.getResponseLimitKind(), resp.ChunkLimit);
        TS_ASSERT_EQUALS(resp.getResponseEncoding(), resp.IdentityEncoding);
        TS_ASSERT_EQUALS(resp.getResponseLength(), 0U);
        TS_ASSERT_EQUALS(resp.getResponseOffset(), 0U);
        TS_ASSERT_EQUALS(resp.getTotalLength(), 0U);
        TS_ASSERT(resp.isKeepalive());
    }

    {
        // No delimiter
        afl::net::http::ClientResponse resp(false);
        afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.1 200 OK\r\n"
                                                            "\r\n"
                                                            "foobar").toBytes());
        bool ok = resp.handleData(bytes);
        TS_ASSERT(ok);
        TS_ASSERT(!resp.hasErrors());
        TS_ASSERT(resp.hasBody());
        TS_ASSERT_EQUALS(resp.getStatusCode(), 200U);
        TS_ASSERT_EQUALS(resp.getStatusPhrase(), "OK");
        TS_ASSERT_EQUALS(resp.getVersion(), "HTTP/1.1");
        TS_ASSERT_EQUALS(resp.getResponseLimitKind(), resp.StreamLimit);
        TS_ASSERT_EQUALS(resp.getResponseEncoding(), resp.IdentityEncoding);
        TS_ASSERT_EQUALS(resp.getResponseLength(), 0U);
        TS_ASSERT_EQUALS(resp.getResponseOffset(), 0U);
        TS_ASSERT_EQUALS(resp.getTotalLength(), 0U);
        TS_ASSERT(!resp.isKeepalive());
    }
}

/** Test bad responses. */
void
TestNetHttpClientResponse::testError()
{
    {
        // Bad intro
        afl::net::http::ClientResponse resp(false);
        afl::base::ConstBytes_t bytes(afl::string::toBytes("Hi mom!\r\n"));
        bool ok = resp.handleData(bytes);
        TS_ASSERT(ok);
        TS_ASSERT(resp.hasErrors());
    }

    {
        // Bad intro
        afl::net::http::ClientResponse resp(false);
        afl::base::ConstBytes_t bytes(afl::string::toBytes("lol\r\n"));
        bool ok = resp.handleData(bytes);
        TS_ASSERT(ok);
        TS_ASSERT(resp.hasErrors());
    }

    {
        // Overlong version
        afl::net::http::ClientResponse resp(false);
        afl::base::ConstBytes_t bytes(afl::string::toBytes("HTTP/3.1415926535897932384626433832795028841968 200 OK\r\n"));
        bool ok = resp.handleData(bytes);
        TS_ASSERT(ok);
        TS_ASSERT(resp.hasErrors());
    }

    {
        // Bad code
        afl::net::http::ClientResponse resp(false);
        afl::base::ConstBytes_t bytes(afl::string::toBytes("200 OK\r\n"));
        bool ok = resp.handleData(bytes);
        TS_ASSERT(ok);
        TS_ASSERT(resp.hasErrors());
    }

    {
        // Bad header
        afl::net::http::ClientResponse resp(false);
        afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.0 200 OK\r\n"
                                                            " foo : bar\r\n\r\n").toBytes());
        bool ok = resp.handleData(bytes);
        TS_ASSERT(ok);
        TS_ASSERT(resp.hasErrors());
    }

    {
        // Partial (parse does not finish)
        afl::net::http::ClientResponse resp(false);
        afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.0 200 OK\r\n"
                                                            "Content-Length: 3\r\n").toBytes());
        bool ok = resp.handleData(bytes);
        TS_ASSERT(!ok);
    }
}

/** Test user headers. */
void
TestNetHttpClientResponse::testUserHeaders()
{
    // Basic
    {
        afl::net::http::ClientResponse resp(false);
        afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.0 200 OK\r\n"
                                                            "Content-Type: text/plain\r\n"
                                                            "Server: whatever\r\n"
                                                            "Content-Length: 0\r\n\r\n").toBytes());
        bool ok = resp.handleData(bytes);
        TS_ASSERT(ok);

        const afl::net::HeaderField* f = resp.getResponseHeaders().get("content-type");
        TS_ASSERT(f != 0);
        TS_ASSERT_EQUALS(f->getName(), "Content-Type");
        TS_ASSERT_EQUALS(f->getValue(), "text/plain");

        f = resp.getResponseHeaders().get("server");
        TS_ASSERT(f != 0);
        TS_ASSERT_EQUALS(f->getName(), "Server");
        TS_ASSERT_EQUALS(f->getValue(), "whatever");
    }

    // Duplicate header
    {
        afl::net::http::ClientResponse resp(false);
        afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.0 200 OK\r\n"
                                                            "Content-Type: text/plain\r\n"
                                                            "Content-Type: whatever\r\n"
                                                            "Content-Length: 0\r\n\r\n").toBytes());
        bool ok = resp.handleData(bytes);
        TS_ASSERT(ok);

        const afl::net::HeaderField* f = resp.getResponseHeaders().get("content-type");
        TS_ASSERT(f != 0);
        TS_ASSERT_EQUALS(f->getName(), "Content-Type");
        TS_ASSERT_EQUALS(f->getValue(), "text/plain");

        f = resp.getResponseHeaders().get("server");
        TS_ASSERT(f == 0);
    }

    // Cookie (example from RFC 6265)
    {
        afl::net::http::ClientResponse resp(false);
        afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.0 200 OK\r\n"
                                                            "Set-Cookie: SID=31d4d96e407aad42; Path=/; Secure; HttpOnly\r\n"
                                                            "Set-Cookie: lang=en-US; Path=/; Domain=example.com\r\n"
                                                            "Content-Type: text/plain\r\n"
                                                            "Content-Length: 0\r\n\r\n").toBytes());
        bool ok = resp.handleData(bytes);
        TS_ASSERT(ok);

        const afl::net::HeaderField* f = resp.getResponseHeaders().get("content-type");
        TS_ASSERT(f != 0);
        TS_ASSERT_EQUALS(f->getName(), "Content-Type");
        TS_ASSERT_EQUALS(f->getValue(), "text/plain");

        f = resp.getResponseCookies().get("sid");
        TS_ASSERT(f != 0);
        TS_ASSERT_EQUALS(f->getName(), "SID");
        TS_ASSERT_EQUALS(f->getPrimaryValue(), "31d4d96e407aad42");
        TS_ASSERT_EQUALS(f->getValue(), "31d4d96e407aad42; Path=/; Secure; HttpOnly");

        String_t tmp;
        TS_ASSERT(f->getSecondaryValue("path", tmp));
        TS_ASSERT_EQUALS(tmp, "/");
        TS_ASSERT(f->getSecondaryValue("httponly", tmp));
        TS_ASSERT_EQUALS(tmp, "");

        f = resp.getResponseCookies().get("lang");
        TS_ASSERT(f != 0);
        TS_ASSERT_EQUALS(f->getPrimaryValue(), "en-US");
    }
}

/** Test keepalive handling, HTTP 1.0. */
void
TestNetHttpClientResponse::testKeepalive()
{
    // No content-length, so cannot be keepalive
    {
        afl::net::http::ClientResponse resp(false);
        afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.0 200 OK\r\n"
                                                            "\r\n").toBytes());
        bool ok = resp.handleData(bytes);
        TS_ASSERT(ok);
        TS_ASSERT(!resp.isKeepalive());
    }

    // Content-length given, but opt-in required for 1.0
    {
        afl::net::http::ClientResponse resp(false);
        afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.0 200 OK\r\n"
                                                            "Content-Length: 3\r\n"
                                                            "\r\n").toBytes());
        bool ok = resp.handleData(bytes);
        TS_ASSERT(ok);
        TS_ASSERT(!resp.isKeepalive());
    }

    // Content-length given, but opt-in required for 1.0
    {
        afl::net::http::ClientResponse resp(false);
        afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.0 200 OK\r\n"
                                                            "Content-Length: 3\r\n"
                                                            "Connection: keep-alive\r\n"
                                                            "\r\n").toBytes());
        bool ok = resp.handleData(bytes);
        TS_ASSERT(ok);
        TS_ASSERT(resp.isKeepalive());
    }

    // Content-length given, bogus Connection header
    {
        afl::net::http::ClientResponse resp(false);
        afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.0 200 OK\r\n"
                                                            "Content-Length: 3\r\n"
                                                            "Connection: lol\r\n"
                                                            "\r\n").toBytes());
        bool ok = resp.handleData(bytes);
        TS_ASSERT(ok);
        TS_ASSERT(!resp.isKeepalive());
    }
}

/** Test keepalive handling, HTTP 1.1. */
void
TestNetHttpClientResponse::testKeepalive11()
{
    // No content-length, so cannot be keepalive
    {
        afl::net::http::ClientResponse resp(false);
        afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.1 200 OK\r\n"
                                                            "\r\n").toBytes());
        bool ok = resp.handleData(bytes);
        TS_ASSERT(ok);
        TS_ASSERT(!resp.isKeepalive());
    }

    // Content-length given, so ok
    {
        afl::net::http::ClientResponse resp(false);
        afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.1 200 OK\r\n"
                                                            "Content-Length: 3\r\n"
                                                            "\r\n").toBytes());
        bool ok = resp.handleData(bytes);
        TS_ASSERT(ok);
        TS_ASSERT(resp.isKeepalive());
    }

    // Content-length given, but opt-out
    {
        afl::net::http::ClientResponse resp(false);
        afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.1 200 OK\r\n"
                                                            "Content-Length: 3\r\n"
                                                            "Connection: close\r\n"
                                                            "\r\n").toBytes());
        bool ok = resp.handleData(bytes);
        TS_ASSERT(ok);
        TS_ASSERT(!resp.isKeepalive());
    }

    // Case-insensitive
    {
        afl::net::http::ClientResponse resp(false);
        afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.1 200 OK\r\n"
                                                            "CONTENT-LENGTH: 3\r\n"
                                                            "CONNECTION: CLOSE\r\n"
                                                            "\r\n").toBytes());
        bool ok = resp.handleData(bytes);
        TS_ASSERT(ok);
        TS_ASSERT(!resp.isKeepalive());
    }

    // Content-length given, bogus Connection header
    {
        afl::net::http::ClientResponse resp(false);
        afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.1 200 OK\r\n"
                                                            "Content-Length: 3\r\n"
                                                            "Connection: lol\r\n"
                                                            "\r\n").toBytes());
        bool ok = resp.handleData(bytes);
        TS_ASSERT(ok);
        TS_ASSERT(resp.isKeepalive());
    }
}

/** Test Content-Encoding. */
void
TestNetHttpClientResponse::testContentEncoding()
{
    // Default
    {
        afl::net::http::ClientResponse resp(false);
        afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.1 200 OK\r\n"
                                                            "Content-Length: 3\r\n"
                                                            "\r\n").toBytes());
        bool ok = resp.handleData(bytes);
        TS_ASSERT(ok);
        TS_ASSERT_EQUALS(resp.getResponseEncoding(), afl::net::http::ClientResponse::IdentityEncoding);
    }

    // gzip
    {
        afl::net::http::ClientResponse resp(false);
        afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.1 200 OK\r\n"
                                                            "Content-Encoding: gzip\r\n"
                                                            "\r\n").toBytes());
        bool ok = resp.handleData(bytes);
        TS_ASSERT(ok);
        TS_ASSERT_EQUALS(resp.getResponseEncoding(), afl::net::http::ClientResponse::GzipEncoding);
    }

    // deflate
    {
        afl::net::http::ClientResponse resp(false);
        afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.1 200 OK\r\n"
                                                            "Content-Encoding: deflate\r\n"
                                                            "\r\n").toBytes());
        bool ok = resp.handleData(bytes);
        TS_ASSERT(ok);
        TS_ASSERT_EQUALS(resp.getResponseEncoding(), afl::net::http::ClientResponse::DeflateEncoding);
    }

    // case-insensitive!
    {
        afl::net::http::ClientResponse resp(false);
        afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.1 200 OK\r\n"
                                                            "CONTENT-ENCODING: DEFLATE\r\n"
                                                            "\r\n").toBytes());
        bool ok = resp.handleData(bytes);
        TS_ASSERT(ok);
        TS_ASSERT_EQUALS(resp.getResponseEncoding(), afl::net::http::ClientResponse::DeflateEncoding);
    }

    // unknown
    {
        afl::net::http::ClientResponse resp(false);
        afl::base::ConstBytes_t bytes(afl::string::toMemory("HTTP/1.1 200 OK\r\n"
                                                            "Content-Encoding: 1337\r\n"
                                                            "\r\n").toBytes());
        bool ok = resp.handleData(bytes);
        TS_ASSERT(ok);
        TS_ASSERT_EQUALS(resp.getResponseEncoding(), afl::net::http::ClientResponse::UnknownEncoding);
    }
}

