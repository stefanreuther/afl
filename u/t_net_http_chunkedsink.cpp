/**
  *  \file u/t_net_http_chunkedsink.cpp
  *  \brief Test for afl::net::http::ChunkedSink
  */

#include "afl/net/http/chunkedsink.hpp"

#include "u/t_net_http.hpp"
#include "afl/string/string.hpp"

namespace {
    class TestSink : public afl::io::DataSink {
     public:
        bool handleData(afl::base::ConstBytes_t& data)
            {
                m_text.append(reinterpret_cast<const char*>(data.unsafeData()), data.size());
                data.reset();
                return false;
            }

        String_t m_text;
    };

    const char TRIVIAL[] = "0\r\n\r\n";

    const char SIMPLE[] =
        "5\r\n"
        "abcde\r\n"
        "0\r\n\r\n";

    const char REMAIN[] =
        "5\r\n"
        "abcde\r\n"
        "0\r\n\r\n"
        "other";

    const char TWO_PARTS[] =
        "5\r\n"
        "abcde\r\n"
        "3\r\n"
        "fgh\r\n"
        "0\r\n\r\n";

    const char EXTENSION[] =
        "5;wow!\r\n"
        "abcde\r\n"
        "3;such extension!\r\n"
        "fgh\r\n"
        "0\r\n"
        "Very: HTTP\r\n\r\n";
}

/** Simple tests for different combinations. */
void
TestNetHttpChunkedSink::testIt()
{
    {
        // Trivial case
        TestSink t;
        afl::net::http::ChunkedSink cs(t);
        afl::base::ConstBytes_t data = afl::string::toBytes(TRIVIAL);
        TS_ASSERT(cs.handleData(data));
        TS_ASSERT(data.empty());
        TS_ASSERT_EQUALS(t.m_text, "");
    }
    {
        // Not so trivial
        TestSink t;
        afl::net::http::ChunkedSink cs(t);
        afl::base::ConstBytes_t data = afl::string::toBytes(SIMPLE);
        TS_ASSERT(cs.handleData(data));
        TS_ASSERT(data.empty());
        TS_ASSERT_EQUALS(t.m_text, "abcde");
    }
    {
        // Data remains
        TestSink t;
        afl::net::http::ChunkedSink cs(t);
        afl::base::ConstBytes_t data = afl::string::toBytes(REMAIN);
        TS_ASSERT(cs.handleData(data));
        TS_ASSERT(!data.empty());
        TS_ASSERT_EQUALS(t.m_text, "abcde");
        TS_ASSERT_EQUALS(data.size(), 5U);
        TS_ASSERT_SAME_DATA(data.unsafeData(), "other", 5);
    }
    {
        // Two parts
        TestSink t;
        afl::net::http::ChunkedSink cs(t);
        afl::base::ConstBytes_t data = afl::string::toBytes(TWO_PARTS);
        TS_ASSERT(cs.handleData(data));
        TS_ASSERT(data.empty());
        TS_ASSERT_EQUALS(t.m_text, "abcdefgh");
    }
    {
        // Extension
        TestSink t;
        afl::net::http::ChunkedSink cs(t);
        afl::base::ConstBytes_t data = afl::string::toBytes(EXTENSION);
        TS_ASSERT(cs.handleData(data));
        TS_ASSERT(data.empty());
        TS_ASSERT_EQUALS(t.m_text, "abcdefgh");
    }

    {
        // Incomplete - Two parts
        afl::base::ConstBytes_t data = afl::string::toBytes(TWO_PARTS);
        for (size_t i = 0; i < data.size(); ++i) {
            TestSink t;
            afl::net::http::ChunkedSink cs(t);
            afl::base::ConstBytes_t part(data.subrange(0, i));
            TS_ASSERT(!cs.handleData(part));
            TS_ASSERT(part.empty());
            part = data.subrange(i);
            TS_ASSERT(cs.handleData(part));
            TS_ASSERT(part.empty());
            TS_ASSERT_EQUALS(t.m_text, "abcdefgh");
        }
    }
    {
        // Incomplete - Extension
        afl::base::ConstBytes_t data = afl::string::toBytes(EXTENSION);
        for (size_t i = 0; i < data.size(); ++i) {
            TestSink t;
            afl::net::http::ChunkedSink cs(t);
            afl::base::ConstBytes_t part(data.subrange(0, i));
            TS_ASSERT(!cs.handleData(part));
            TS_ASSERT(part.empty());
            part = data.subrange(i);
            TS_ASSERT(cs.handleData(part));
            TS_ASSERT(part.empty());
            TS_ASSERT_EQUALS(t.m_text, "abcdefgh");
        }
    }
}
