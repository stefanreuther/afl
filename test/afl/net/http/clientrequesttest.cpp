/**
  *  \file test/afl/net/http/clientrequesttest.cpp
  *  \brief Test for afl::net::http::ClientRequest
  */

#include "afl/net/http/clientrequest.hpp"
#include "afl/test/testrunner.hpp"

using afl::net::http::ClientRequest;

AFL_TEST("afl.net.http.ClientRequest:interface", a)
{
    // Test interface instance
    class Tester : public ClientRequest {
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
    a.checkEqual("getRequestId init", t.getRequestId(), 0U);

    t.setRequestId(999999999);
    a.checkEqual("getRequestId set", t.getRequestId(), 999999999U);
}

AFL_TEST("afl.net.http.ClientRequest:toString", a)
{
    a.checkDifferent("01", toString(ClientRequest::Cancelled),           "");
    a.checkDifferent("02", toString(ClientRequest::ConnectionFailed),    "");
    a.checkDifferent("03", toString(ClientRequest::ConnectionClosed),    "");
    a.checkDifferent("04", toString(ClientRequest::UnsupportedProtocol), "");
    a.checkDifferent("05", toString(ClientRequest::NetworkError),        "");
    a.checkDifferent("06", toString(ClientRequest::ServerError),         "");
}
