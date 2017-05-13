/**
  *  \file u/t_net_http_pagerequest.cpp
  *  \brief Test for afl::net::http::PageRequest
  */

#include "afl/net/http/pagerequest.hpp"
#include "u/t_net_http.hpp"

/** Test initialisation and accessors. */
void
TestNetHttpPageRequest::testAccessors()
{
    // Initialisation
    afl::net::http::PageRequest rq("/a", "/b", "/c?d=e");
    TS_ASSERT_EQUALS(rq.getRootPath(), "/a");
    TS_ASSERT_EQUALS(rq.getSelfPath(), "/b");
    TS_ASSERT_EQUALS(rq.getPath(), "/c");
    TS_ASSERT(rq.arguments().get("e") == 0);
    TS_ASSERT(rq.arguments().get("d") != 0);
    TS_ASSERT_EQUALS(rq.arguments().get("d")->getValue(), "e");
    TS_ASSERT_EQUALS(rq.getMethod(), "GET");

    // Modification
    rq.setRootPath("/x");
    rq.setSelfPath("/y/z");
    rq.setPath("/qq");
    rq.setMethod("NOTIFY");
    TS_ASSERT_EQUALS(rq.getRootPath(), "/x");
    TS_ASSERT_EQUALS(rq.getSelfPath(), "/y/z");
    TS_ASSERT_EQUALS(rq.getPath(), "/qq");
    TS_ASSERT_EQUALS(rq.getMethod(), "NOTIFY");

    // setPath() does not parse
    rq.setPath("/qq?a=1");
    TS_ASSERT_EQUALS(rq.getPath(), "/qq?a=1");

    // Headers
    TS_ASSERT(rq.headers().get("foo") == 0);
    TS_ASSERT(rq.headers().get("FOO") == 0);
    rq.headers().add("foo", "bar");
    TS_ASSERT(rq.headers().get("foo") != 0);
    TS_ASSERT(rq.headers().get("FOO") != 0);
    TS_ASSERT_EQUALS(rq.headers().get("foo")->getValue(), "bar");
}

/** Another parser test. */
void
TestNetHttpPageRequest::testUrlParse()
{
    afl::net::http::PageRequest rq("/a", "/b", "?d=e");
    TS_ASSERT_EQUALS(rq.getRootPath(), "/a");
    TS_ASSERT_EQUALS(rq.getSelfPath(), "/b");
    TS_ASSERT_EQUALS(rq.getPath(), "");
    TS_ASSERT(rq.arguments().get("e") == 0);
    TS_ASSERT(rq.arguments().get("d") != 0);
    TS_ASSERT_EQUALS(rq.arguments().get("d")->getValue(), "e");
}

/** Test body handling. */
void
TestNetHttpPageRequest::testBody()
{
    static const uint8_t BODY[] = { 1,2,3,4 };

    // GET has no body, so we ignore it
    {
        afl::net::http::PageRequest rq("/a", "/b", "");
        rq.handleData(BODY);
        rq.finish();
        TS_ASSERT_EQUALS(rq.body().getContent().size(), 0U);
    }

    // POST has a body with an appropriate content type
    {
        afl::net::http::PageRequest rq("/a", "/b", "");
        rq.setMethod("POST");
        rq.headers().add("Content-Type", "application/octet-stream");
        rq.handleData(BODY);
        rq.finish();
        TS_ASSERT_EQUALS(rq.body().getContent().size(), 4U);
        TS_ASSERT(rq.body().getContent().compareContent(BODY));
    }

    // POST form data
    {
        afl::net::http::PageRequest rq("/a", "/b", "");
        rq.setMethod("POST");
        rq.headers().add("Content-Type", "application/x-www-form-urlencoded");
        rq.handleData(afl::string::toBytes("a=1&b=2&c=%3D"));
        rq.finish();

        // Body is empty because it has been parsed!
        TS_ASSERT_EQUALS(rq.body().getContent().size(), 0U);

        // Check arguments
        TS_ASSERT(rq.arguments().get("a") != 0);
        TS_ASSERT_EQUALS(rq.arguments().get("a")->getValue(), "1");
        TS_ASSERT(rq.arguments().get("b") != 0);
        TS_ASSERT_EQUALS(rq.arguments().get("b")->getValue(), "2");
        TS_ASSERT(rq.arguments().get("c") != 0);
        TS_ASSERT_EQUALS(rq.arguments().get("c")->getValue(), "=");
    }
}
