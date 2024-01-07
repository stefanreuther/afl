/**
  *  \file test/afl/net/headerfieldtest.cpp
  *  \brief Test for afl::net::HeaderField
  */

#include "afl/net/headerfield.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.net.HeaderField:default", a)
{
    afl::net::HeaderField f;
    a.checkEqual("name", f.getName(), "");
    a.checkEqual("value", f.getValue(), "");
    a.checkEqual("prim", f.getPrimaryValue(), "");
}

AFL_TEST("afl.net.HeaderField:value-with-attributes", a)
{
    // Typical Content-Type
    String_t tmp;
    afl::net::HeaderField f("Content-Type", "text/plain; charset=UTF-8");
    a.checkEqual("name", f.getName(), "Content-Type");
    a.checkEqual("value", f.getValue(), "text/plain; charset=UTF-8");
    a.checkEqual("prim", f.getPrimaryValue(), "text/plain");
    a.check("secondary 1", f.getSecondaryValue("charset", tmp));
    a.checkEqual("result 1", tmp, "UTF-8");
    a.check("secondary 2", f.getSecondaryValue("CHARSET", tmp));
    a.checkEqual("result 2", tmp, "UTF-8");
    a.check("secondary 3", !f.getSecondaryValue("chars", tmp));
}

AFL_TEST("afl.net.HeaderField:value-with-quoted-attributes", a)
{
    // Same thing, with quoting
    String_t tmp;
    afl::net::HeaderField f("Content-Type", "\"text/plain\"; charset=\"UTF-8\"; extra=\"\\\"\"");
    a.checkEqual("name", f.getName(), "Content-Type");
    a.checkEqual("prim", f.getPrimaryValue(), "text/plain");
    a.check("secondary 1", f.getSecondaryValue("charset", tmp));
    a.checkEqual("result 1", tmp, "UTF-8");
    a.check("secondary 2", f.getSecondaryValue("extra", tmp));
    a.checkEqual("result 2", tmp, "\"");
}

AFL_TEST("afl.net.HeaderField:email-address", a)
{
    // Typical mail address
    afl::net::HeaderField f("From", "user@dom.ain (Rea L. Name)");
    a.checkEqual("name", f.getName(), "From");
    a.checkEqual("value", f.getValue(), "user@dom.ain (Rea L. Name)");
    a.checkEqual("prim", f.getPrimaryValue(), "user@dom.ain");
}
AFL_TEST("afl.net.HeaderField:blank-fields", a)
{
    // Cookie example from RFC 6265 using blank fields
    String_t tmp;
    afl::net::HeaderField f("SID", "31d4d96e407aad42; Path=/; Secure; HttpOnly");
    a.checkEqual("name", f.getName(), "SID");
    a.checkEqual("prim", f.getPrimaryValue(), "31d4d96e407aad42");
    a.check("secondary 1", f.getSecondaryValue("path", tmp));
    a.checkEqual("value 1", tmp, "/");
    a.check("secondary 2", f.getSecondaryValue("secure", tmp));
    a.checkEqual("value 2", tmp, "");
    a.check("secondary 3", f.getSecondaryValue("httponly", tmp));
    a.checkEqual("value 3", tmp, "");
}

AFL_TEST("afl.net.HeaderField:flags:NoComments", a)
{
    // Preserve comments
    afl::net::HeaderField f("From", "user@dom.ain (Rea L. Name)");
    a.checkEqual("name", f.getName(), "From");
    a.checkEqual("value", f.getValue(), "user@dom.ain (Rea L. Name)");
    a.checkEqual("prim", f.getPrimaryValue(f.NoComments), "user@dom.ain (Rea L. Name)");
}

AFL_TEST("afl.net.HeaderField:flags:NoComments+NoQuotes", a)
{
    // Quoted ";"
    String_t tmp;
    afl::net::HeaderField f("User-Agent", "Acme/1.0 (\";-); OS/2; CP/M)");
    a.checkEqual("name", f.getName(), "User-Agent");
    a.checkEqual("value", f.getValue(), "Acme/1.0 (\";-); OS/2; CP/M)");
    a.checkEqual("prim", f.getPrimaryValue(),                            "Acme/1.0");
    a.checkEqual("prim NoComments", f.getPrimaryValue(f.NoComments),                "Acme/1.0 (;-); OS/2; CP/M)");
    a.checkEqual("prim NoQuotes",   f.getPrimaryValue(f.NoQuotes),                  "Acme/1.0");
    a.checkEqual("prim NoCom+Quot", f.getPrimaryValue(f.NoQuotes | f.NoComments),   "Acme/1.0 (\"");
}

AFL_TEST("afl.net.HeaderField:flags:NoPrimary", a)
{
    // No primary value
    afl::net::HeaderField f("Cookie", "login=1; layout=2");
    a.checkEqual("name", f.getName(), "Cookie");
    a.checkEqual("value", f.getValue(), "login=1; layout=2");

    // NoPrimary has no effect on getPrimaryValue()
    a.checkEqual("prim",           f.getPrimaryValue(),            "login=1");
    a.checkEqual("prim NoPrimary", f.getPrimaryValue(f.NoPrimary), "login=1");

    // Individual values
    String_t tmp;
    a.check("secondary 1", !f.getSecondaryValue("login", tmp, 0));
    a.checkEqual("value 1", tmp, "");
    a.check("secondary 2", f.getSecondaryValue("login", tmp, f.NoPrimary));
    a.checkEqual("value 2", tmp, "1");
    a.check("secondary 3", f.getSecondaryValue("layout", tmp, 0));
    a.checkEqual("value 3", tmp, "2");
    a.check("secondary 4", f.getSecondaryValue("layout", tmp, f.NoPrimary));
    a.checkEqual("value 4", tmp, "2");
}

/** Test getAddressValue(). */
AFL_TEST("afl.net.HeaderField:getAddressValue", a)
{
    String_t tmp;

    // Valid forms
    a.check("01. status", afl::net::HeaderField("From", "user@host").getAddressValue(tmp));
    a.checkEqual("02. result", tmp, "user@host");

    a.check("11. status", afl::net::HeaderField("From", "  a@b.c (A. B)  ").getAddressValue(tmp));
    a.checkEqual("12. result", tmp, "a@b.c");

    a.check("21. status", afl::net::HeaderField("From", "A. B <x@y.z>").getAddressValue(tmp));
    a.checkEqual("22. result", tmp, "x@y.z");

    // Borderline valid: multiple elements. Will return first element.
    a.check("31. status", afl::net::HeaderField("From", "a@b, c@d").getAddressValue(tmp));
    a.checkEqual("32. result", tmp, "a@b");
    a.check("41. status", afl::net::HeaderField("From", "e@f (g), c@d (y)").getAddressValue(tmp));
    a.checkEqual("42. result", tmp, "e@f");
    a.check("51. status", afl::net::HeaderField("From", "a <a@b>, c <c@d>").getAddressValue(tmp));
    a.checkEqual("52. result", tmp, "a@b");

    // Invalid
    a.check("61. invalid", !afl::net::HeaderField("From", "q").getAddressValue(tmp));
    a.check("62. invalid", !afl::net::HeaderField("From", "a b@c").getAddressValue(tmp));
}
