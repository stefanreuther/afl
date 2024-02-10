/**
  *  \file test/afl/net/http/simpledownloadlistenertest.cpp
  *  \brief Test for afl::net::http::SimpleDownloadListener
  */

#include "afl/net/http/simpledownloadlistener.hpp"

#include "afl/test/testrunner.hpp"

using afl::net::http::SimpleDownloadListener;

AFL_TEST("afl.net.http.SimpleDownloadListener:toString", a)
{
    a.checkDifferent("01", toString(SimpleDownloadListener::Succeeded),     "");
    a.checkDifferent("02", toString(SimpleDownloadListener::Failed),        "");
    a.checkDifferent("03", toString(SimpleDownloadListener::TimedOut),      "");
    a.checkDifferent("04", toString(SimpleDownloadListener::LimitExceeded), "");
}

