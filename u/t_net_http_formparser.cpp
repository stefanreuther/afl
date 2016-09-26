/**
  *  \file u/t_net_http_formparser.cpp
  *  \brief Test for afl::net::http::FormParser
  */

#include "afl/net/http/formparser.hpp"

#include "u/t_net_http.hpp"

/** Simple tests. */
void
TestNetHttpFormParser::testIt()
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
    p.handleData("<Test>", data);
    p.handleDataComplete();
    TS_ASSERT_EQUALS(thc.m_result, "");

    thc.m_result = "";
    data = afl::string::toBytes("x");
    p.handleData("<Test>", data);
    p.handleDataComplete();
    TS_ASSERT_EQUALS(thc.m_result, "<<x>>(())");

    thc.m_result = "";
    data = afl::string::toBytes("x&y&");
    p.handleData("<Test>", data);
    p.handleDataComplete();
    TS_ASSERT_EQUALS(thc.m_result, "<<x>>(())<<y>>(())");

    thc.m_result = "";
    data = afl::string::toBytes("x=y");
    p.handleData("<Test>", data);
    p.handleDataComplete();
    TS_ASSERT_EQUALS(thc.m_result, "<<x>>((y))");

    thc.m_result = "";
    data = afl::string::toBytes("x=y&z");
    p.handleData("<Test>", data);
    p.handleDataComplete();
    TS_ASSERT_EQUALS(thc.m_result, "<<x>>((y))<<z>>(())");

    thc.m_result = "";
    data = afl::string::toBytes("x=a+b+c");
    p.handleData("<Test>", data);
    p.handleDataComplete();
    TS_ASSERT_EQUALS(thc.m_result, "<<x>>((a b c))");

    thc.m_result = "";
    data = afl::string::toBytes("=a");
    p.handleData("<Test>", data);
    p.handleDataComplete();
    TS_ASSERT_EQUALS(thc.m_result, "<<>>((a))");

    thc.m_result = "";
    data = afl::string::toBytes("a=%3D%25");
    p.handleData("<Test>", data);
    p.handleDataComplete();
    TS_ASSERT_EQUALS(thc.m_result, "<<a>>((=%))");

    thc.m_result = "";
    data = afl::string::toBytes("%3D=%25");
    p.handleData("<Test>", data);
    p.handleDataComplete();
    TS_ASSERT_EQUALS(thc.m_result, "<<=>>((%))");

    thc.m_result = "";
    data = afl::string::toBytes("p=/foo");
    p.handleData("<Test>", data);
    p.handleDataComplete();
    TS_ASSERT_EQUALS(thc.m_result, "<<p>>((/foo))");

    thc.m_result = "";
    data = afl::string::toBytes("p=/foo?p=/foo");
    p.handleData("<Test>", data);
    p.handleDataComplete();
    TS_ASSERT_EQUALS(thc.m_result, "<<p>>((/foo?p=/foo))");

    thc.m_result = "";
    data = afl::string::toBytes("p=/foo?p=/foo&a=1");
    p.handleData("<Test>", data);
    p.handleDataComplete();
    TS_ASSERT_EQUALS(thc.m_result, "<<p>>((/foo?p=/foo))<<a>>((1))");
}
