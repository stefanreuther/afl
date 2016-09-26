/**
  *  \file u/t_net_name.cpp
  *  \brief Test for afl::net::Name
  */

#include "afl/net/name.hpp"

#include "u/t_net.hpp"

/** Test initialisation. */
void
TestNetName::testInit()
{
    {
        afl::net::Name x("host", "port");
        TS_ASSERT_EQUALS(x.getName(), "host");
        TS_ASSERT_EQUALS(x.getService(), "port");
        TS_ASSERT(x.isValid());

        x.setName("a");
        TS_ASSERT_EQUALS(x.getName(), "a");
        TS_ASSERT_EQUALS(x.getService(), "port");
        TS_ASSERT(x.isValid());

        x.setService("b");
        TS_ASSERT_EQUALS(x.getName(), "a");
        TS_ASSERT_EQUALS(x.getService(), "b");
        TS_ASSERT(x.isValid());
    }

    {
        afl::net::Name x;
        TS_ASSERT_EQUALS(x.getName(), "");
        TS_ASSERT_EQUALS(x.getService(), "");
        TS_ASSERT(!x.isValid());

        x.setName("x");
        TS_ASSERT(!x.isValid());

        x.setService("b");
        TS_ASSERT(x.isValid());
    }

    {
        afl::net::Name x("host", 80);
        TS_ASSERT_EQUALS(x.getName(), "host");
        TS_ASSERT_EQUALS(x.getService(), "80");
    }

    {
        afl::net::Name x("host", 45454);
        TS_ASSERT_EQUALS(x.getName(), "host");
        TS_ASSERT_EQUALS(x.getService(), "45454");

        x.setService(32);
        TS_ASSERT_EQUALS(x.getService(), "32");

        x.setService(55555);
        TS_ASSERT_EQUALS(x.getService(), "55555");
    }
}

/** Test parsing. */
void
TestNetName::testParse()
{
    afl::net::Name x = afl::net::Name::parse("localhost:8181", "80");
    TS_ASSERT_EQUALS(x.getName(), "localhost");
    TS_ASSERT_EQUALS(x.getService(), "8181");
    TS_ASSERT_EQUALS(x.toString(), "localhost:8181");

    x = afl::net::Name::parse("127.0.0.1:8181", "80");
    TS_ASSERT_EQUALS(x.getName(), "127.0.0.1");
    TS_ASSERT_EQUALS(x.getService(), "8181");
    TS_ASSERT_EQUALS(x.toString(), "127.0.0.1:8181");

    x = afl::net::Name::parse("[::1]:8181", "80");
    TS_ASSERT_EQUALS(x.getName(), "::1");
    TS_ASSERT_EQUALS(x.getService(), "8181");
    TS_ASSERT_EQUALS(x.toString(), "[::1]:8181");

    x = afl::net::Name::parse("localhost", "80");
    TS_ASSERT_EQUALS(x.getName(), "localhost");
    TS_ASSERT_EQUALS(x.getService(), "80");
    TS_ASSERT_EQUALS(x.toString(), "localhost:80");

    x = afl::net::Name::parse("127.0.0.1", "80");
    TS_ASSERT_EQUALS(x.getName(), "127.0.0.1");
    TS_ASSERT_EQUALS(x.getService(), "80");
    TS_ASSERT_EQUALS(x.toString(), "127.0.0.1:80");

    x = afl::net::Name::parse("[::1]", "80");
    TS_ASSERT_EQUALS(x.getName(), "::1");
    TS_ASSERT_EQUALS(x.getService(), "80");
    TS_ASSERT_EQUALS(x.toString(), "[::1]:80");
}

/** Test comparisons. */
void
TestNetName::testCompare()
{
    afl::net::Name a, b, c("localhost", 80), d("localhost", "80"), e("localhost", "http");
    TS_ASSERT_EQUALS(a, b);
    TS_ASSERT_EQUALS(c, d);
    TS_ASSERT_DIFFERS(a, c);
    TS_ASSERT_DIFFERS(a, e);
    TS_ASSERT_DIFFERS(c, e);
}
