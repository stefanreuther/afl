/**
  *  \file test/afl/net/parameterencodertest.cpp
  *  \brief Test for afl::net::ParameterEncoder
  */

#include "afl/net/parameterencoder.hpp"

#include "afl/net/headertable.hpp"
#include "afl/test/testrunner.hpp"

/** Test behaviour with empty HeaderTable. */
AFL_TEST("afl.net.ParameterEncoder:empty", a)
{
    // Prepare
    afl::net::HeaderTable empty;

    // Tests
    {
        String_t s;
        afl::net::ParameterEncoder enc(s);
        empty.enumerateHeaders(enc);
        a.checkEqual("case 1", s, "");
    }
    {
        String_t s("foo");
        afl::net::ParameterEncoder enc(s);
        empty.enumerateHeaders(enc);
        a.checkEqual("case 2", s, "foo");
    }
    {
        String_t s;
        afl::net::ParameterEncoder enc(s, '\0');
        empty.enumerateHeaders(enc);
        a.checkEqual("case 3", s, "");
    }
    {
        String_t s("foo");
        afl::net::ParameterEncoder enc(s, '\0');
        empty.enumerateHeaders(enc);
        a.checkEqual("case 4", s, "foo");
    }
    {
        String_t s("foo");
        afl::net::ParameterEncoder enc(s, '?');
        empty.enumerateHeaders(enc);
        a.checkEqual("case 5", s, "foo");
    }
    {
        String_t s("foo");
        afl::net::ParameterEncoder enc(s, '&');
        empty.enumerateHeaders(enc);
        a.checkEqual("case 6", s, "foo");
    }
}

/** Test behaviour with one-element HeaderTable. */
AFL_TEST("afl.net.ParameterEncoder:one-element", a)
{
    // Prepare
    afl::net::HeaderTable one;
    one.add("key", "value");

    // Tests
    {
        String_t s;
        afl::net::ParameterEncoder enc(s);
        one.enumerateHeaders(enc);
        a.checkEqual("case 1", s, "?key=value");
    }
    {
        String_t s("foo");
        afl::net::ParameterEncoder enc(s);
        one.enumerateHeaders(enc);
        a.checkEqual("case 2", s, "foo?key=value");
    }
    {
        String_t s;
        afl::net::ParameterEncoder enc(s, '\0');
        one.enumerateHeaders(enc);
        a.checkEqual("case 3", s, "key=value");
    }
    {
        String_t s("foo");
        afl::net::ParameterEncoder enc(s, '\0');
        one.enumerateHeaders(enc);
        a.checkEqual("case 4", s, "fookey=value");
    }
    {
        String_t s("foo");
        afl::net::ParameterEncoder enc(s, '?');
        one.enumerateHeaders(enc);
        a.checkEqual("case 5", s, "foo?key=value");
    }
    {
        String_t s("foo");
        afl::net::ParameterEncoder enc(s, '&');
        one.enumerateHeaders(enc);
        a.checkEqual("case 6", s, "foo&key=value");
    }
}

/** Test behaviour with two-element HeaderTable. */
AFL_TEST("afl.net.ParameterEncoder:two-elements", a)
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
        a.checkEqual("case 1", s, "?key=value&more=stuff");
    }
    {
        String_t s("foo");
        afl::net::ParameterEncoder enc(s);
        two.enumerateHeaders(enc);
        a.checkEqual("case 2", s, "foo?key=value&more=stuff");
    }
    {
        String_t s;
        afl::net::ParameterEncoder enc(s, '\0');
        two.enumerateHeaders(enc);
        a.checkEqual("case 3", s, "key=value&more=stuff");
    }
    {
        String_t s("foo");
        afl::net::ParameterEncoder enc(s, '\0');
        two.enumerateHeaders(enc);
        a.checkEqual("case 4", s, "fookey=value&more=stuff");
    }
    {
        String_t s("foo");
        afl::net::ParameterEncoder enc(s, '?');
        two.enumerateHeaders(enc);
        a.checkEqual("case 5", s, "foo?key=value&more=stuff");
    }
    {
        String_t s("foo");
        afl::net::ParameterEncoder enc(s, '&');
        two.enumerateHeaders(enc);
        a.checkEqual("case 6", s, "foo&key=value&more=stuff");
    }
}

/** Test behaviour with characters that need encoding. */
AFL_TEST("afl.net.ParameterEncoder:escaped-characters", a)
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
    a.checkEqual("", s, "?a=a&b=%82%CF&c=a%2Bb&d=x%25y&e=p%26q&f=m%3Dn&g=hi+there&e%82%CFe=e");
}
