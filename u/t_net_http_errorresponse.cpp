/**
  *  \file u/t_net_http_errorresponse.cpp
  *  \brief Test for afl::net::http::ErrorResponse
  */

#include "afl/net/http/errorresponse.hpp"

#include "t_net_http.hpp"
#include "afl/net/http/request.hpp"

/** Basic test. */
void
TestNetHttpErrorResponse::testIt()
{
    // Build a request
    std::auto_ptr<afl::net::http::Request> req(new afl::net::http::Request());
    afl::base::ConstBytes_t bytes = afl::string::toBytes("GET / HTTP/1.0\r\n"
                                                         "Host: wherever\r\n\r\n");
    bool ok = req->handleData("data", bytes);
    TS_ASSERT(ok);
    TS_ASSERT(bytes.empty());

    // Build an error response
    afl::net::http::ErrorResponse testee(req, "500 Server error");

    // Must ignore additional data
    bytes = afl::string::toBytes("xyz");
    TS_ASSERT(!testee.handleData("data", bytes));
    TS_ASSERT_EQUALS(bytes.size(), 3U);

    // Not keepalive
    TS_ASSERT(!testee.isKeepalive());

    // Coverage...
    testee.handleDataComplete();

    // Get data
    bytes = testee.getData();
    TS_ASSERT(bytes.size() > 19U);
    TS_ASSERT_SAME_DATA(bytes.unsafeData(), "HTTP/1.0 500 Server", 19);

    // Get more data -> fails, we already sent
    bytes = testee.getData();
    TS_ASSERT(bytes.empty());
}
