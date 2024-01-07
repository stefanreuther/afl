/**
  *  \file test/afl/net/nullnetworkstacktest.cpp
  *  \brief Test for afl::net::NullNetworkStack
  */

#include "afl/net/nullnetworkstack.hpp"

#include "afl/except/fileproblemexception.hpp"
#include "afl/net/name.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.net.NullNetworkStack", a)
{
    afl::net::NullNetworkStack testee;

    AFL_CHECK_THROWS(a("listen"),    testee.listen(afl::net::Name("127.0.0.1", "9999"), 10),   afl::except::FileProblemException);
    AFL_CHECK_THROWS(a("connect/1"), testee.connect(afl::net::Name("127.0.0.1", "9999")),      afl::except::FileProblemException);
    AFL_CHECK_THROWS(a("connect/2"), testee.connect(afl::net::Name("127.0.0.1", "9999"), 10),  afl::except::FileProblemException);
}
