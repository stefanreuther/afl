/**
  *  \file test/afl/net/http/downloadlistenertest.cpp
  *  \brief Test for afl::net::http::DownloadListener
  */

#include "afl/net/http/downloadlistener.hpp"
#include "afl/test/testrunner.hpp"

/** Interface test. */
AFL_TEST_NOARG("afl.net.http.DownloadListener")
{
    class Tester : public afl::net::http::DownloadListener {
     public:
        virtual void handleResponseHeader(afl::net::http::ClientResponse& /*resp*/)
            { }
        virtual void handleResponseData(afl::io::Stream::FileSize_t /*ofs*/, afl::base::ConstBytes_t /*data*/)
            { }
        virtual void handleFailure(afl::net::http::ClientRequest::FailureReason /*reason*/, const String_t& /*message*/)
            { }
        virtual void handleSuccess()
            { }
    };
    Tester t;
}
