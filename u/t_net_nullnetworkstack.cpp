/**
  *  \file u/t_net_nullnetworkstack.cpp
  *  \brief Test for afl::net::NullNetworkStack
  */

#include "afl/net/nullnetworkstack.hpp"

#include "t_net.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/net/name.hpp"

void
TestNetNullNetworkStack::testIt()
{
    afl::net::NullNetworkStack testee;

    TS_ASSERT_THROWS(testee.listen(afl::net::Name("127.0.0.1", "9999"), 10),   afl::except::FileProblemException);
    TS_ASSERT_THROWS(testee.connect(afl::net::Name("127.0.0.1", "9999")),      afl::except::FileProblemException);
    TS_ASSERT_THROWS(testee.connect(afl::net::Name("127.0.0.1", "9999"), 10),  afl::except::FileProblemException);
}
