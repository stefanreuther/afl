/**
  *  \file test/afl/net/http/pagerequesttest.cpp
  *  \brief Test for afl::net::http::PageRequest
  */

#include "afl/net/http/pagerequest.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.net.http.PageRequest:accessors", a)
{
    // Initialisation
    afl::net::http::PageRequest rq("/a", "/b", "/c?d=e");
    a.checkEqual("01. getRootPath", rq.getRootPath(), "/a");
    a.checkEqual("02. getSelfPath", rq.getSelfPath(), "/b");
    a.checkEqual("03. getPath", rq.getPath(), "/c");
    a.checkNull("04. args get e", rq.arguments().get("e"));
    a.checkNonNull("05. args get d", rq.arguments().get("d"));
    a.checkEqual("06. args d value", rq.arguments().get("d")->getValue(), "e");
    a.checkEqual("07. getMethod", rq.getMethod(), "GET");

    // Modification
    rq.setRootPath("/x");
    rq.setSelfPath("/y/z");
    rq.setPath("/qq");
    rq.setMethod("NOTIFY");
    a.checkEqual("21. getRootPath", rq.getRootPath(), "/x");
    a.checkEqual("22. getSelfPath", rq.getSelfPath(), "/y/z");
    a.checkEqual("23. getPath", rq.getPath(), "/qq");
    a.checkEqual("24. getMethod", rq.getMethod(), "NOTIFY");

    // setPath() does not parse
    rq.setPath("/qq?a=1");
    a.checkEqual("31. getPath", rq.getPath(), "/qq?a=1");

    // Headers
    a.checkNull("41. header get", rq.headers().get("foo"));
    a.checkNull("42. header get", rq.headers().get("FOO"));
    rq.headers().add("foo", "bar");
    a.checkNonNull("43. header get", rq.headers().get("foo"));
    a.checkNonNull("44. header get", rq.headers().get("FOO"));
    a.checkEqual("45. header value", rq.headers().get("foo")->getValue(), "bar");
}

/** Another parser test. */
AFL_TEST("afl.net.http.PageRequest:parse", a)
{
    afl::net::http::PageRequest rq("/a", "/b", "?d=e");
    a.checkEqual("getRootPath", rq.getRootPath(), "/a");
    a.checkEqual("getSelfPath", rq.getSelfPath(), "/b");
    a.checkEqual("getPath", rq.getPath(), "");
    a.check("args e", rq.arguments().get("e") == 0);
    a.check("args d", rq.arguments().get("d") != 0);
    a.checkEqual("arg value", rq.arguments().get("d")->getValue(), "e");
}

/*
 *  Body handling
 */

// GET has no body, so we ignore it
AFL_TEST("afl.net.http.PageRequest:body:get", a)
{
    static const uint8_t BODY[] = { 1,2,3,4 };
    afl::net::http::PageRequest rq("/a", "/b", "");
    rq.handleData(BODY);
    rq.finish();
    a.checkEqual("content size", rq.body().getContent().size(), 0U);
}

// POST has a body with an appropriate content type
AFL_TEST("afl.net.http.PageRequest:body:post-binary", a)
{
    static const uint8_t BODY[] = { 1,2,3,4 };
    afl::net::http::PageRequest rq("/a", "/b", "");
    rq.setMethod("POST");
    rq.headers().add("Content-Type", "application/octet-stream");
    rq.handleData(BODY);
    rq.finish();
    a.checkEqual("content size", rq.body().getContent().size(), 4U);
    a.checkEqual("content data", rq.body().getContent().compareContent(BODY), 0);
}

// POST form data
AFL_TEST("afl.net.http.PageRequest:body:post-form", a)
{
    afl::net::http::PageRequest rq("/a", "/b", "");
    rq.setMethod("POST");
    rq.headers().add("Content-Type", "application/x-www-form-urlencoded");
    rq.handleData(afl::string::toBytes("a=1&b=2&c=%3D"));
    rq.finish();

    // Body is empty because it has been parsed!
    a.checkEqual("content size", rq.body().getContent().size(), 0U);

    // Check arguments
    a.checkNonNull("arg a", rq.arguments().get("a"));
    a.checkEqual("value a", rq.arguments().get("a")->getValue(), "1");
    a.checkNonNull("arg b", rq.arguments().get("b"));
    a.checkEqual("value b", rq.arguments().get("b")->getValue(), "2");
    a.checkNonNull("arg c", rq.arguments().get("c"));
    a.checkEqual("value c", rq.arguments().get("c")->getValue(), "=");
}
