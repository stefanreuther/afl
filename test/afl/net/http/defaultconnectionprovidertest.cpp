/**
  *  \file test/afl/net/http/defaultconnectionprovidertest.cpp
  *  \brief Test for afl::net::http::DefaultConnectionProvider
  */

#include "afl/net/http/defaultconnectionprovider.hpp"

#include "afl/test/testrunner.hpp"
#include "afl/net/http/client.hpp"

/** Simple test. */
AFL_TEST_NOARG("afl.net.http.DefaultConnectionProvider")
{
    /* This one is pretty complicated to test fully because we'd
       have to mock a full HTTP server.
       So, unit-test just basic integration and setup/shutdown,
       actual functionality will be tested manually (wget example). */
    afl::net::http::Client client;
    afl::net::http::DefaultConnectionProvider provider(client);
    provider.requestNewConnection();
}
