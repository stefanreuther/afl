/**
  *  \file test/afl/net/nametest.cpp
  *  \brief Test for afl::net::Name
  */

#include "afl/net/name.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.net.Name:init:str-str", a)
{
    afl::net::Name x("host", "port");
    a.checkEqual("getName", x.getName(), "host");
    a.checkEqual("getService", x.getService(), "port");
    a.check("isValid", x.isValid());

    x.setName("a");
    a.checkEqual("getName 2", x.getName(), "a");
    a.checkEqual("getService 2", x.getService(), "port");
    a.check("isValid 2", x.isValid());

    x.setService("b");
    a.checkEqual("getName 3", x.getName(), "a");
    a.checkEqual("getService 3", x.getService(), "b");
    a.check("isValid 3", x.isValid());
}

AFL_TEST("afl.net.Name:init:default", a)
{
    afl::net::Name x;
    a.checkEqual("getName", x.getName(), "");
    a.checkEqual("getService", x.getService(), "");
    a.check("isValid 1", !x.isValid());

    x.setName("x");
    a.check("isValid 2", !x.isValid());

    x.setService("b");
    a.check("isValid 3", x.isValid());
}

AFL_TEST("afl.net.Name:init:str-int", a)
{
    afl::net::Name x("host", 80);
    a.checkEqual("getName", x.getName(), "host");
    a.checkEqual("getService", x.getService(), "80");
}

AFL_TEST("afl.net.Name:init:str-uint", a)
{
    afl::net::Name x("host", 45454);
    a.checkEqual("getName", x.getName(), "host");
    a.checkEqual("getService", x.getService(), "45454");

    x.setService(32);
    a.checkEqual("getService 2", x.getService(), "32");

    x.setService(55555);
    a.checkEqual("getService 3", x.getService(), "55555");
}

/** Test parsing. */
AFL_TEST("afl.net.Name:parse:name+port", a)
{
    afl::net::Name x = afl::net::Name::parse("localhost:8181", "80");
    a.checkEqual("getName", x.getName(), "localhost");
    a.checkEqual("getService", x.getService(), "8181");
    a.checkEqual("toString", x.toString(), "localhost:8181");
}

AFL_TEST("afl.net.Name:parse:ipv4+port", a)
{
    afl::net::Name x = afl::net::Name::parse("127.0.0.1:8181", "80");
    a.checkEqual("getName", x.getName(), "127.0.0.1");
    a.checkEqual("getService", x.getService(), "8181");
    a.checkEqual("toString", x.toString(), "127.0.0.1:8181");
}

AFL_TEST("afl.net.Name:parse:ipv6+port", a)
{
    afl::net::Name x = afl::net::Name::parse("[::1]:8181", "80");
    a.checkEqual("getName", x.getName(), "::1");
    a.checkEqual("getService", x.getService(), "8181");
    a.checkEqual("toString", x.toString(), "[::1]:8181");
}

AFL_TEST("afl.net.Name:parse:name", a)
{
    afl::net::Name x = afl::net::Name::parse("localhost", "80");
    a.checkEqual("getName", x.getName(), "localhost");
    a.checkEqual("getService", x.getService(), "80");
    a.checkEqual("toString", x.toString(), "localhost:80");
}

AFL_TEST("afl.net.Name:parse:ipv4", a)
{
    afl::net::Name x = afl::net::Name::parse("127.0.0.1", "80");
    a.checkEqual("getName", x.getName(), "127.0.0.1");
    a.checkEqual("getService", x.getService(), "80");
    a.checkEqual("toString", x.toString(), "127.0.0.1:80");
}

AFL_TEST("afl.net.Name:parse:ipv6", a)
{
    afl::net::Name x = afl::net::Name::parse("[::1]", "80");
    a.checkEqual("getName", x.getName(), "::1");
    a.checkEqual("getService", x.getService(), "80");
    a.checkEqual("toString", x.toString(), "[::1]:80");
}

/** Test comparisons. */
AFL_TEST("afl.net.Name:compare", as)
{
    afl::net::Name a, b, c("localhost", 80), d("localhost", "80"), e("localhost", "http");
    as.check("", a == b);
    as.check("", c == d);
    as.check("", !(a == c));
    as.check("", !(a == e));
    as.check("", !(c == e));
}
