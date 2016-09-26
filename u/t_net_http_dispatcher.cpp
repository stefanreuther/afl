/**
  *  \file u/t_net_http_dispatcher.cpp
  *  \brief Test for afl::net::http::Dispatcher
  */

#include "afl/net/http/dispatcher.hpp"

#include "u/t_net_http.hpp"

/** Simple test. */
void
TestNetHttpDispatcher::testIt()
{
    /* This is an interface, so we instantiate it and check whether it works the way it's intented. */
    class Dis : public afl::net::http::Dispatcher {
     public:
        virtual afl::net::http::Response* createNewResponse(std::auto_ptr<afl::net::http::Request>& /*request*/)
            { return 0; }
    };
    Dis d;
}
