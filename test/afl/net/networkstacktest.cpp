/**
  *  \file test/afl/net/networkstacktest.cpp
  *  \brief Test for afl::net::NetworkStack
  */

#include "afl/net/networkstack.hpp"

#include "afl/test/testrunner.hpp"
#include "afl/net/name.hpp"
#include "afl/except/fileproblemexception.hpp"

/** Test connect(). */
AFL_TEST("afl.net.NetworkStack:connect", a)
{
    // Test connect to nonexistant service. It should fail.
    afl::net::NetworkStack& ns = afl::net::NetworkStack::getInstance();
    bool ok = false;
    try {
        afl::net::Name name("localhost", "26");     // 26 is unassigned, we assume nothing runs there
        afl::base::Ptr<afl::net::Socket> sock = ns.connect(name, 1000).asPtr();

        // If it returns, it must be nonzero
        a.checkNonNull("returned socket", sock.get());
        ok = true;
    }
    catch (afl::except::FileProblemException&) {
        // Failed with error, is also ok
        ok = true;
    }
    a.check("correct result", ok);
}

/** Test connect/listen with a bad name. */
AFL_TEST("afl.net.NetworkStack:bad-name", a)
{
    afl::net::NetworkStack& ns = afl::net::NetworkStack::getInstance();
    AFL_CHECK_THROWS(a("invalid host"),    ns.connect(afl::net::Name(String_t("a\0b", 3), "80")),          afl::except::FileProblemException);
    AFL_CHECK_THROWS(a("invalid service"), ns.connect(afl::net::Name("localhost", String_t("8\0""0", 3))), afl::except::FileProblemException);
}
