/**
  *  \file u/t_net_http_clientrequest.cpp
  *  \brief Test for afl::net::http::ClientRequest
  */

#include "afl/net/http/clientrequest.hpp"

#include "u/t_net_http.hpp"

/** Interface test. */
void
TestNetHttpClientRequest::testInterface()
{
    // Test interface instance
    class Tester : public afl::net::http::ClientRequest {
     public:
        virtual afl::net::Name getName() const
            { return afl::net::Name(); }
        virtual String_t getScheme() const
            { return String_t(); }
        virtual bool isHeadRequest() const
            { return false; }
        virtual afl::base::ConstBytes_t getRequestData()
            { return afl::base::ConstBytes_t(); }
        virtual bool restart()
            { return false; }
        virtual void handleResponseHeader(afl::net::http::ClientResponse& /*resp*/)
            { }
        virtual void handleResponseData(afl::base::ConstBytes_t /*data*/)
            { }
        virtual void handleFailure(FailureReason /*reason*/, const String_t& /*message*/)
            { }
        virtual void handleSuccess()
            { }
    };
    Tester t;

    // Test concrete methods (trivial, I know)
    TS_ASSERT_EQUALS(t.getRequestId(), 0U);

    t.setRequestId(999999999);
    TS_ASSERT_EQUALS(t.getRequestId(), 999999999U);
}

