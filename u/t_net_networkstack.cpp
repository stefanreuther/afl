/**
  *  \file u/t_net_networkstack.cpp
  *  \brief Test for afl::net::NetworkStack
  */

#include "afl/net/networkstack.hpp"

#include "u/t_net.hpp"
#include "afl/net/name.hpp"
#include "afl/except/fileproblemexception.hpp"

/** Test connect(). */
void
TestNetNetworkStack::testConnect()
{
    // Test connect to nonexistant service. It should fail.
    afl::net::NetworkStack& ns = afl::net::NetworkStack::getInstance();
    bool ok = false;
    try {
        afl::net::Name name("localhost", "26");     // 26 is unassigned, we assume nothing runs there
        afl::base::Ptr<afl::net::Socket> sock = ns.connect(name, 1000).asPtr();

        // If it returns, it must be nonzero (but failed test...)
        TS_ASSERT(sock.get() != 0);
        ok = true;
    }
    catch (afl::except::FileProblemException&) {
        // Failed with error, is also ok
        ok = true;
    }
    TS_ASSERT(ok);
}

/** Test connect/listen with a bad name. */
void
TestNetNetworkStack::testBadName()
{
    afl::net::NetworkStack& ns = afl::net::NetworkStack::getInstance();
    TS_ASSERT_THROWS(ns.connect(afl::net::Name(String_t("a\0b", 3), "80")),          afl::except::FileProblemException);
    TS_ASSERT_THROWS(ns.connect(afl::net::Name("localhost", String_t("8\0""0", 3))), afl::except::FileProblemException);
}

