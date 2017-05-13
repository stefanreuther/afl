/**
  *  \file u/t_net_http_pageresponse.cpp
  *  \brief Test for afl::net::http::PageResponse
  */

#include "afl/net/http/pageresponse.hpp"
#include "u/t_net_http.hpp"

/** Simple accessor test. */
void
TestNetHttpPageResponse::testAccessors()
{
    // Start successful
    afl::net::http::PageResponse resp;
    TS_ASSERT_EQUALS(resp.getStatusCode(), 200);

    // Make it an error
    resp.setStatusCode(resp.NOT_FOUND);
    TS_ASSERT_EQUALS(resp.getStatusCode(), 404);
    TS_ASSERT_EQUALS(resp.getStatusText(), "Not Found");

    // Set a header
    resp.headers().set("Server", "Test/1.0");
    TS_ASSERT(resp.headers().get("server") != 0);
    TS_ASSERT_EQUALS(resp.headers().get("server")->getValue(), "Test/1.0");

    // Still no body
    TS_ASSERT_EQUALS(resp.body().getContent().size(), 0U);

    // Finish it
    resp.finish();

    // Now it has a body
    TS_ASSERT(resp.body().getContent().size() > 0);

    // Verify headers
    TS_ASSERT(resp.headers().get("server") != 0);
    TS_ASSERT(resp.headers().get("content-length") != 0);
    TS_ASSERT(resp.headers().get("content-type") != 0);
}

/** Test status codes. */
void
TestNetHttpPageResponse::testStatusCodes()
{
    // Test text representation of common codes
    afl::net::http::PageResponse testee;
    TS_ASSERT_EQUALS(testee.getStatusCode(), 200);
    TS_ASSERT_EQUALS(testee.getStatusText(), "OK");

    testee.setStatusCode(302);
    TS_ASSERT_EQUALS(testee.getStatusCode(), 302);
    TS_ASSERT_EQUALS(testee.getStatusText(), "Found");

    testee.setStatusCode(400);
    TS_ASSERT_EQUALS(testee.getStatusCode(), 400);
    TS_ASSERT_EQUALS(testee.getStatusText(), "Bad Request");

    testee.setStatusCode(404);
    TS_ASSERT_EQUALS(testee.getStatusCode(), 404);
    TS_ASSERT_EQUALS(testee.getStatusText(), "Not Found");

    testee.setStatusCode(405);
    TS_ASSERT_EQUALS(testee.getStatusCode(), 405);
    TS_ASSERT_EQUALS(testee.getStatusText(), "Method Not Allowed");

    testee.setStatusCode(500);
    TS_ASSERT_EQUALS(testee.getStatusCode(), 500);
    TS_ASSERT_EQUALS(testee.getStatusText(), "Internal Server Error");

    // No code must return an empty string
    for (int i = -1000; i < 1000; ++i) {
        testee.setStatusCode(i);
        TS_ASSERT(testee.getStatusText() != "");
    }
}

/** Test setRedirect(). */
void
TestNetHttpPageResponse::testRedirect()
{
    afl::net::http::PageResponse testee;
    testee.setRedirect("/foo");

    // Access as const (for coverage...)
    const afl::net::http::PageResponse& resp = testee;
    TS_ASSERT_EQUALS(resp.getStatusCode(), 302);
    TS_ASSERT(resp.headers().get("location") != 0);
    TS_ASSERT_EQUALS(resp.headers().get("location")->getValue(), "/foo");
}

