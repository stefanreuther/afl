/**
  *  \file u/t_net_http.hpp
  *  \brief Test for afl::net::http
  */
#ifndef AFL_U_T_NET_HTTP_HPP
#define AFL_U_T_NET_HTTP_HPP

#include <cxxtest/TestSuite.h>

class TestNetHttpChunkedSink : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestNetHttpClient : public CxxTest::TestSuite {
 public:
    void testStupidServer();
    void testShutdown();
};

class TestNetHttpClientConnection : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestNetHttpClientConnectionProvider : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestNetHttpClientRequest : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestNetHttpClientResponse : public CxxTest::TestSuite {
 public:
    void testOK();
    void testError();
    void testUserHeaders();
};

class TestNetHttpCookie : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestNetHttpCookieJar : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestNetHttpDefaultConnectionProvider : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestNetHttpDispatcher : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestNetHttpDownloadListener : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestNetHttpErrorResponse : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestNetHttpFormParser : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestNetHttpPage : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestNetHttpPageDispatcher : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestNetHttpPageRequest : public CxxTest::TestSuite {
 public:
    void testAccessors();
    void testUrlParse();
    void testBody();
};

class TestNetHttpPageResponse : public CxxTest::TestSuite {
 public:
    void testAccessors();
};

class TestNetHttpRequest : public CxxTest::TestSuite {
 public:
    void testIt();
    void testPath();
};

class TestNetHttpResponse : public CxxTest::TestSuite {
 public:
    void testIt();
};

#endif
