/**
  *  \file u/t_net_smtp_configuration.cpp
  *  \brief Test for afl::net::smtp::Configuration
  */

#include "afl/net/smtp/configuration.hpp"

#include "t_net_smtp.hpp"

/** Simple test. */
void
TestNetSmtpConfiguration::testIt()
{
    afl::net::smtp::Configuration testee("hello-string", "from-string");
    TS_ASSERT_EQUALS(testee.hello, "hello-string");
    TS_ASSERT_EQUALS(testee.from, "from-string");

    afl::net::smtp::Configuration copy(testee);
    TS_ASSERT_EQUALS(copy.hello, "hello-string");
    TS_ASSERT_EQUALS(copy.from, "from-string");
}

