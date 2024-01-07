/**
  *  \file test/afl/net/http/dispatchertest.cpp
  *  \brief Test for afl::net::http::Dispatcher
  */

#include "afl/net/http/dispatcher.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST_NOARG("afl.net.http.Dispatcher")
{
    /* This is an interface, so we instantiate it and check whether it works the way it's intented. */
    class Dis : public afl::net::http::Dispatcher {
     public:
        virtual afl::net::http::Response* createNewResponse(std::auto_ptr<afl::net::http::Request>& /*request*/)
            { return 0; }
    };
    Dis d;
}
