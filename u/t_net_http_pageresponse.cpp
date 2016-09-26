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
