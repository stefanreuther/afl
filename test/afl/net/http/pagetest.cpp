/**
  *  \file test/afl/net/http/pagetest.cpp
  *  \brief Test for afl::net::http::PAge
  */

#include "afl/net/http/page.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST_NOARG("afl.net.http.Page")
{
    /* This is an interface, so we instantiate it and check whether it works the way it's intented. */
    class P : public afl::net::http::Page {
     public:
        virtual bool isValidMethod(const String_t& method) const
            { return method == "GET"; }
        virtual bool isValidPath() const
            { return true; }
        virtual void handleRequest(afl::net::http::PageRequest& /*in*/, afl::net::http::PageResponse& /*out*/)
            { }
    };
    P p;
}
