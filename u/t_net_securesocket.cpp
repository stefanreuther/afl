/**
  *  \file u/t_net_securesocket.cpp
  *  \brief Test for afl::net::SecureSocket
  */

#include "afl/net/securesocket.hpp"
#include "u/t_net.hpp"
#include "afl/base/ptr.hpp"

/** Simple test.
    SecureSocket is just an interface (like Socket).
    It can only be tested with the factory classes (SecureContext etc.).
    Therefore, only check that it can be managed as intended. */
void
TestNetSecureSocket::testIt()
{
    afl::base::Ptr<afl::net::SecureSocket> sock;
}
