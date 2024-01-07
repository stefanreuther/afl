/**
  *  \file test/afl/net/http/pageresponsetest.cpp
  *  \brief Test for afl::net::http::PageResponse
  */

#include "afl/net/http/pageresponse.hpp"
#include "afl/test/testrunner.hpp"

/** Simple accessor test. */
AFL_TEST("afl.net.http.PageResponse:accessors", a)
{
    // Start successful
    afl::net::http::PageResponse resp;
    a.checkEqual("getStatusCode default", resp.getStatusCode(), 200);

    // Make it an error
    resp.setStatusCode(resp.NOT_FOUND);
    a.checkEqual("getStatusCode 404", resp.getStatusCode(), 404);
    a.checkEqual("getStatusText 404", resp.getStatusText(), "Not Found");

    // Set a header
    resp.headers().set("Server", "Test/1.0");
    a.checkNonNull("header get", resp.headers().get("server"));
    a.checkEqual("header value", resp.headers().get("server")->getValue(), "Test/1.0");

    // Still no body
    a.checkEqual("body empty", resp.body().getContent().size(), 0U);

    // Finish it
    resp.finish();

    // Now it has a body
    a.check("body nonempty", resp.body().getContent().size() > 0);

    // Verify headers
    a.checkNonNull("final server header",         resp.headers().get("server"));
    a.checkNonNull("final content-length header", resp.headers().get("content-length"));
    a.checkNonNull("final content-type header",   resp.headers().get("content-type"));
}

/** Test status codes. */
AFL_TEST("afl.net.http.PageResponse:status", a)
{
    // Test text representation of common codes
    afl::net::http::PageResponse testee;
    a.checkEqual("getStatusCode 200", testee.getStatusCode(), 200);
    a.checkEqual("getStatusText 200", testee.getStatusText(), "OK");

    testee.setStatusCode(302);
    a.checkEqual("getStatusCode 302", testee.getStatusCode(), 302);
    a.checkEqual("getStatusText 302", testee.getStatusText(), "Found");

    testee.setStatusCode(400);
    a.checkEqual("getStatusCode 400", testee.getStatusCode(), 400);
    a.checkEqual("getStatusText 400", testee.getStatusText(), "Bad Request");

    testee.setStatusCode(404);
    a.checkEqual("getStatusCode 404", testee.getStatusCode(), 404);
    a.checkEqual("getStatusText 404", testee.getStatusText(), "Not Found");

    testee.setStatusCode(405);
    a.checkEqual("getStatusCode 405", testee.getStatusCode(), 405);
    a.checkEqual("getStatusText 405", testee.getStatusText(), "Method Not Allowed");

    testee.setStatusCode(500);
    a.checkEqual("getStatusCode 500", testee.getStatusCode(), 500);
    a.checkEqual("getStatusText 500", testee.getStatusText(), "Internal Server Error");

    // No code must return an empty string
    for (int i = -1000; i < 1000; ++i) {
        testee.setStatusCode(i);
        a.checkDifferent("getStatusText coverage", testee.getStatusText(), "");
    }
}

/** Test setRedirect(). */
AFL_TEST("afl.net.http.PageResponse:setRedirect", a)
{
    afl::net::http::PageResponse testee;
    testee.setRedirect("/foo");

    // Access as const (for coverage...)
    const afl::net::http::PageResponse& resp = testee;
    a.checkEqual("getStatusCode", resp.getStatusCode(), 302);
    a.checkNonNull("header get", resp.headers().get("location"));
    a.checkEqual("header value", resp.headers().get("location")->getValue(), "/foo");
}
