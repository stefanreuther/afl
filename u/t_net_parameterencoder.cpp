/**
  *  \file u/t_net_parameterencoder.cpp
  *  \brief Test for afl::net::ParameterEncoder
  */

#include "afl/net/parameterencoder.hpp"

#include "u/t_net.hpp"
#include "afl/net/headertable.hpp"

/** Test behaviour with empty HeaderTable. */
void
TestNetParameterEncoder::testEmpty()
{
    // Prepare
    afl::net::HeaderTable empty;

    // Tests
    {
        String_t s;
        afl::net::ParameterEncoder enc(s);
        empty.enumerateHeaders(enc);
        TS_ASSERT_EQUALS(s, "");
    }
    {
        String_t s("foo");
        afl::net::ParameterEncoder enc(s);
        empty.enumerateHeaders(enc);
        TS_ASSERT_EQUALS(s, "foo");
    }
    {
        String_t s;
        afl::net::ParameterEncoder enc(s, '\0');
        empty.enumerateHeaders(enc);
        TS_ASSERT_EQUALS(s, "");
    }
    {
        String_t s("foo");
        afl::net::ParameterEncoder enc(s, '\0');
        empty.enumerateHeaders(enc);
        TS_ASSERT_EQUALS(s, "foo");
    }
    {
        String_t s("foo");
        afl::net::ParameterEncoder enc(s, '?');
        empty.enumerateHeaders(enc);
        TS_ASSERT_EQUALS(s, "foo");
    }
    {
        String_t s("foo");
        afl::net::ParameterEncoder enc(s, '&');
        empty.enumerateHeaders(enc);
        TS_ASSERT_EQUALS(s, "foo");
    }
}

/** Test behaviour with one-element HeaderTable. */
void
TestNetParameterEncoder::testOne()
{
    // Prepare
    afl::net::HeaderTable one;
    one.add("key", "value");

    // Tests
    {
        String_t s;
        afl::net::ParameterEncoder enc(s);
        one.enumerateHeaders(enc);
        TS_ASSERT_EQUALS(s, "?key=value");
    }
    {
        String_t s("foo");
        afl::net::ParameterEncoder enc(s);
        one.enumerateHeaders(enc);
        TS_ASSERT_EQUALS(s, "foo?key=value");
    }
    {
        String_t s;
        afl::net::ParameterEncoder enc(s, '\0');
        one.enumerateHeaders(enc);
        TS_ASSERT_EQUALS(s, "key=value");
    }
    {
        String_t s("foo");
        afl::net::ParameterEncoder enc(s, '\0');
        one.enumerateHeaders(enc);
        TS_ASSERT_EQUALS(s, "fookey=value");
    }
    {
        String_t s("foo");
        afl::net::ParameterEncoder enc(s, '?');
        one.enumerateHeaders(enc);
        TS_ASSERT_EQUALS(s, "foo?key=value");
    }
    {
        String_t s("foo");
        afl::net::ParameterEncoder enc(s, '&');
        one.enumerateHeaders(enc);
        TS_ASSERT_EQUALS(s, "foo&key=value");
    }
}

/** Test behaviour with two-element HeaderTable. */
void
TestNetParameterEncoder::testTwo()
{
    // Prepare
    afl::net::HeaderTable two;
    two.add("key", "value");
    two.add("more", "stuff");

    // Tests
    {
        String_t s;
        afl::net::ParameterEncoder enc(s);
        two.enumerateHeaders(enc);
        TS_ASSERT_EQUALS(s, "?key=value&more=stuff");
    }
    {
        String_t s("foo");
        afl::net::ParameterEncoder enc(s);
        two.enumerateHeaders(enc);
        TS_ASSERT_EQUALS(s, "foo?key=value&more=stuff");
    }
    {
        String_t s;
        afl::net::ParameterEncoder enc(s, '\0');
        two.enumerateHeaders(enc);
        TS_ASSERT_EQUALS(s, "key=value&more=stuff");
    }
    {
        String_t s("foo");
        afl::net::ParameterEncoder enc(s, '\0');
        two.enumerateHeaders(enc);
        TS_ASSERT_EQUALS(s, "fookey=value&more=stuff");
    }
    {
        String_t s("foo");
        afl::net::ParameterEncoder enc(s, '?');
        two.enumerateHeaders(enc);
        TS_ASSERT_EQUALS(s, "foo?key=value&more=stuff");
    }
    {
        String_t s("foo");
        afl::net::ParameterEncoder enc(s, '&');
        two.enumerateHeaders(enc);
        TS_ASSERT_EQUALS(s, "foo&key=value&more=stuff");
    }
}

/** Test behaviour with characters that need encoding. */
void
TestNetParameterEncoder::testEncoding()
{
    // Prepare some headers
    afl::net::HeaderTable t;
    t.add("a", "a");
    t.add("b", "\x82\xCF");
    t.add("c", "a+b");
    t.add("d", "x%y");
    t.add("e", "p&q");
    t.add("f", "m=n");
    t.add("g", "hi there");
    t.add("e\x82\xCF""e", "e");

    String_t s;
    afl::net::ParameterEncoder enc(s);
    t.enumerateHeaders(enc);
    TS_ASSERT_EQUALS(s, "?a=a&b=%82%CF&c=a%2Bb&d=x%25y&e=p%26q&f=m%3Dn&g=hi+there&e%82%CFe=e");
}
