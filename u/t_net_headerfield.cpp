/**
  *  \file u/t_net_headerfield.cpp
  *  \brief Test for afl::net::HeaderField
  */

#include "afl/net/headerfield.hpp"

#include "u/t_net.hpp"

/** Test normal operations. */
void
TestNetHeaderField::testNormal()
{
    {
        afl::net::HeaderField f;
        TS_ASSERT_EQUALS(f.getName(), "");
        TS_ASSERT_EQUALS(f.getValue(), "");
        TS_ASSERT_EQUALS(f.getPrimaryValue(), "");
    }
    {
        // Typical Content-Type
        String_t tmp;
        afl::net::HeaderField f("Content-Type", "text/plain; charset=UTF-8");
        TS_ASSERT_EQUALS(f.getName(), "Content-Type");
        TS_ASSERT_EQUALS(f.getValue(), "text/plain; charset=UTF-8");
        TS_ASSERT_EQUALS(f.getPrimaryValue(), "text/plain");
        TS_ASSERT(f.getSecondaryValue("charset", tmp));
        TS_ASSERT_EQUALS(tmp, "UTF-8");
        TS_ASSERT(f.getSecondaryValue("CHARSET", tmp));
        TS_ASSERT_EQUALS(tmp, "UTF-8");
        TS_ASSERT(!f.getSecondaryValue("chars", tmp));
    }
    {
        // Same thing, with quoting
        String_t tmp;
        afl::net::HeaderField f("Content-Type", "\"text/plain\"; charset=\"UTF-8\"; extra=\"\\\"\"");
        TS_ASSERT_EQUALS(f.getName(), "Content-Type");
        TS_ASSERT_EQUALS(f.getPrimaryValue(), "text/plain");
        TS_ASSERT(f.getSecondaryValue("charset", tmp));
        TS_ASSERT_EQUALS(tmp, "UTF-8");
        TS_ASSERT(f.getSecondaryValue("extra", tmp));
        TS_ASSERT_EQUALS(tmp, "\"");
    }
    {
        // Typical mail address
        afl::net::HeaderField f("From", "user@dom.ain (Rea L. Name)");
        TS_ASSERT_EQUALS(f.getName(), "From");
        TS_ASSERT_EQUALS(f.getValue(), "user@dom.ain (Rea L. Name)");
        TS_ASSERT_EQUALS(f.getPrimaryValue(), "user@dom.ain");
    }
    {
        // Cookie example from RFC 6265 using blank fields
        String_t tmp;
        afl::net::HeaderField f("SID", "31d4d96e407aad42; Path=/; Secure; HttpOnly");
        TS_ASSERT_EQUALS(f.getName(), "SID");
        TS_ASSERT_EQUALS(f.getPrimaryValue(), "31d4d96e407aad42");
        TS_ASSERT(f.getSecondaryValue("path", tmp));
        TS_ASSERT_EQUALS(tmp, "/");
        TS_ASSERT(f.getSecondaryValue("secure", tmp));
        TS_ASSERT_EQUALS(tmp, "");
        TS_ASSERT(f.getSecondaryValue("httponly", tmp));
        TS_ASSERT_EQUALS(tmp, "");
    }
}

/** Test special flags. */
void
TestNetHeaderField::testSpecial()
{
    {
        // Preserve comments
        afl::net::HeaderField f("From", "user@dom.ain (Rea L. Name)");
        TS_ASSERT_EQUALS(f.getName(), "From");
        TS_ASSERT_EQUALS(f.getValue(), "user@dom.ain (Rea L. Name)");
        TS_ASSERT_EQUALS(f.getPrimaryValue(f.NoComments), "user@dom.ain (Rea L. Name)");
    }
    {
        // Quoted ";"
        String_t tmp;
        afl::net::HeaderField f("User-Agent", "Acme/1.0 (\";-); OS/2; CP/M)");
        TS_ASSERT_EQUALS(f.getName(), "User-Agent");
        TS_ASSERT_EQUALS(f.getValue(), "Acme/1.0 (\";-); OS/2; CP/M)");
        TS_ASSERT_EQUALS(f.getPrimaryValue(),                            "Acme/1.0");
        TS_ASSERT_EQUALS(f.getPrimaryValue(f.NoComments),                "Acme/1.0 (;-); OS/2; CP/M)");
        TS_ASSERT_EQUALS(f.getPrimaryValue(f.NoQuotes),                  "Acme/1.0");
        TS_ASSERT_EQUALS(f.getPrimaryValue(f.NoQuotes | f.NoComments),   "Acme/1.0 (\"");
    }
}

/** Test NoPrimary flag. */
void
TestNetHeaderField::testNoPrimary()
{
    // No primary value
    afl::net::HeaderField f("Cookie", "login=1; layout=2");
    TS_ASSERT_EQUALS(f.getName(), "Cookie");
    TS_ASSERT_EQUALS(f.getValue(), "login=1; layout=2");

    // NoPrimary has no effect on getPrimaryValue()
    TS_ASSERT_EQUALS(f.getPrimaryValue(),            "login=1");
    TS_ASSERT_EQUALS(f.getPrimaryValue(f.NoPrimary), "login=1");

    // Individual values
    String_t tmp;
    TS_ASSERT(!f.getSecondaryValue("login", tmp, 0));
    TS_ASSERT_EQUALS(tmp, "");
    TS_ASSERT(f.getSecondaryValue("login", tmp, f.NoPrimary));
    TS_ASSERT_EQUALS(tmp, "1");
    TS_ASSERT(f.getSecondaryValue("layout", tmp, 0));
    TS_ASSERT_EQUALS(tmp, "2");
    TS_ASSERT(f.getSecondaryValue("layout", tmp, f.NoPrimary));
    TS_ASSERT_EQUALS(tmp, "2");
}

/** Test getAddressValue(). */
void
TestNetHeaderField::testAddress()
{
    String_t tmp;

    // Valid forms
    TS_ASSERT(afl::net::HeaderField("From", "user@host").getAddressValue(tmp));
    TS_ASSERT_EQUALS(tmp, "user@host");

    TS_ASSERT(afl::net::HeaderField("From", "  a@b.c (A. B)  ").getAddressValue(tmp));
    TS_ASSERT_EQUALS(tmp, "a@b.c");

    TS_ASSERT(afl::net::HeaderField("From", "A. B <x@y.z>").getAddressValue(tmp));
    TS_ASSERT_EQUALS(tmp, "x@y.z");

    // Borderline valid: multiple elements. Will return first element.
    TS_ASSERT(afl::net::HeaderField("From", "a@b, c@d").getAddressValue(tmp));
    TS_ASSERT_EQUALS(tmp, "a@b");
    TS_ASSERT(afl::net::HeaderField("From", "e@f (g), c@d (y)").getAddressValue(tmp));
    TS_ASSERT_EQUALS(tmp, "e@f");
    TS_ASSERT(afl::net::HeaderField("From", "a <a@b>, c <c@d>").getAddressValue(tmp));
    TS_ASSERT_EQUALS(tmp, "a@b");

    // Invalid
    TS_ASSERT(!afl::net::HeaderField("From", "q").getAddressValue(tmp));
    TS_ASSERT(!afl::net::HeaderField("From", "a b@c").getAddressValue(tmp));
}

