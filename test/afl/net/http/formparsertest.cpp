/**
  *  \file test/afl/net/http/formparsertest.cpp
  *  \brief Test for afl::net::http::FormParser
  */

#include "afl/net/http/formparser.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.net.http.FormParser", a)
{
    class TestHeaderConsumer : public afl::net::HeaderConsumer {
     public:
        virtual void handleHeader(String_t key, String_t value)
            {
                m_result += "<<";
                m_result += key;
                m_result += ">>";
                m_result += "((";
                m_result += value;
                m_result += "))";
            }
        String_t m_result;
    };
    TestHeaderConsumer thc;
    afl::base::ConstBytes_t data;
    afl::net::http::FormParser p(thc);

    // The following are the same test cases as for Url::matchArguments.
    thc.m_result = "";
    data = afl::base::ConstBytes_t();
    p.handleData(data);
    p.handleDataComplete();
    a.checkEqual("01. result", thc.m_result, "");

    thc.m_result = "";
    data = afl::string::toBytes("x");
    p.handleData(data);
    p.handleDataComplete();
    a.checkEqual("02. result", thc.m_result, "<<x>>(())");

    thc.m_result = "";
    data = afl::string::toBytes("x&y&");
    p.handleData(data);
    p.handleDataComplete();
    a.checkEqual("03. result", thc.m_result, "<<x>>(())<<y>>(())");

    thc.m_result = "";
    data = afl::string::toBytes("x=y");
    p.handleData(data);
    p.handleDataComplete();
    a.checkEqual("04. result", thc.m_result, "<<x>>((y))");

    thc.m_result = "";
    data = afl::string::toBytes("x=y&z");
    p.handleData(data);
    p.handleDataComplete();
    a.checkEqual("05. result", thc.m_result, "<<x>>((y))<<z>>(())");

    thc.m_result = "";
    data = afl::string::toBytes("x=a+b+c");
    p.handleData(data);
    p.handleDataComplete();
    a.checkEqual("06. result", thc.m_result, "<<x>>((a b c))");

    thc.m_result = "";
    data = afl::string::toBytes("=a");
    p.handleData(data);
    p.handleDataComplete();
    a.checkEqual("07. result", thc.m_result, "<<>>((a))");

    thc.m_result = "";
    data = afl::string::toBytes("a=%3D%25");
    p.handleData(data);
    p.handleDataComplete();
    a.checkEqual("08. result", thc.m_result, "<<a>>((=%))");

    thc.m_result = "";
    data = afl::string::toBytes("%3D=%25");
    p.handleData(data);
    p.handleDataComplete();
    a.checkEqual("09. result", thc.m_result, "<<=>>((%))");

    thc.m_result = "";
    data = afl::string::toBytes("p=/foo");
    p.handleData(data);
    p.handleDataComplete();
    a.checkEqual("10. result", thc.m_result, "<<p>>((/foo))");

    thc.m_result = "";
    data = afl::string::toBytes("p=/foo?p=/foo");
    p.handleData(data);
    p.handleDataComplete();
    a.checkEqual("11. result", thc.m_result, "<<p>>((/foo?p=/foo))");

    thc.m_result = "";
    data = afl::string::toBytes("p=/foo?p=/foo&a=1");
    p.handleData(data);
    p.handleDataComplete();
    a.checkEqual("12. result", thc.m_result, "<<p>>((/foo?p=/foo))<<a>>((1))");
}
