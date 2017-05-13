/**
  *  \file u/t_net_http_response.cpp
  *  \brief Test for afl::net::http::Response
  */

#include "afl/net/http/response.hpp"

#include "u/t_net_http.hpp"

/** Simple test. */
void
TestNetHttpResponse::testIt()
{
    /* This is an interface, so we instantiate it and check whether it works the way it's intented. */
    class Resp : public afl::net::http::Response {
     public:
        virtual bool handleData(afl::base::ConstBytes_t& /*data*/)
            { return false; }
        virtual void handleDataComplete()
            { }
        virtual bool isKeepalive()
            { return true; }
        virtual afl::base::ConstBytes_t getData()
            { return afl::base::ConstBytes_t(); }
    };
    Resp r;
}
