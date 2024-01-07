/**
  *  \file test/afl/net/securesockettest.cpp
  *  \brief Test for afl::net::SecureSocket
  */

#include "afl/net/securesocket.hpp"

#include "afl/base/ptr.hpp"
#include "afl/test/testrunner.hpp"

/** Simple test.
    SecureSocket is just an interface (like Socket).
    It can only be tested with the factory classes (SecureContext etc.).
    Therefore, only check that it can be managed as intended. */
AFL_TEST_NOARG("afl.net.SecureSocket")
{
    afl::base::Ptr<afl::net::SecureSocket> sock;
}
