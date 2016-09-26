/**
  *  \file u/t_net_http_page.cpp
  *  \brief Test for afl::net::http::Page
  */

#include "u/t_net_http.hpp"
#include "afl/net/http/page.hpp"

/** Simple test. */
void
TestNetHttpPage::testIt()
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
