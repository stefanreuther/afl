/**
  *  \file test/afl/net/smtp/configurationtest.cpp
  *  \brief Test for afl::net::smtp::Configuration
  */

#include "afl/net/smtp/configuration.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.net.smtp.Configuration", a)
{
    afl::net::smtp::Configuration testee("hello-string", "from-string");
    a.checkEqual("hello", testee.hello, "hello-string");
    a.checkEqual("from", testee.from, "from-string");

    afl::net::smtp::Configuration copy(testee);
    a.checkEqual("hello 2", copy.hello, "hello-string");
    a.checkEqual("from 2", copy.from, "from-string");
}
