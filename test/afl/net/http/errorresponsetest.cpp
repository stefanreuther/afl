/**
  *  \file test/afl/net/http/errorresponsetest.cpp
  *  \brief Test for afl::net::http::ErrorResponse
  */

#include "afl/net/http/errorresponse.hpp"

#include "afl/net/http/request.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.net.http.ErrorResponse", a)
{
    // Build a request
    std::auto_ptr<afl::net::http::Request> req(new afl::net::http::Request());
    afl::base::ConstBytes_t bytes = afl::string::toBytes("GET / HTTP/1.0\r\n"
                                                         "Host: wherever\r\n\r\n");
    bool ok = req->handleData(bytes);
    a.check("request handleData ok", ok);
    a.check("request complete", bytes.empty());

    // Build an error response
    afl::net::http::ErrorResponse testee(req, "500 Server error");

    // Must ignore additional data
    bytes = afl::string::toBytes("xyz");
    a.check("response reject payload", !testee.handleData(bytes));
    a.checkEqual("payload bytes remaining", bytes.size(), 3U);

    // Not keepalive
    a.check("response isKeepalive", !testee.isKeepalive());

    // Coverage...
    testee.handleDataComplete();

    // Get data
    bytes = testee.getData();
    a.check("response data", bytes.size() > 19U);
    a.checkEqualContent("response data content", bytes.subrange(0, 19), afl::string::toBytes("HTTP/1.0 500 Server"));

    // Get more data -> fails, we already sent
    bytes = testee.getData();
    a.check("response data ends", bytes.empty());
}
