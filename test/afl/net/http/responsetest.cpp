/**
  *  \file test/afl/net/http/responsetest.cpp
  *  \brief Test for afl::net::http::Response
  */

#include "afl/net/http/response.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST_NOARG("afl.net.http.Response")
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
