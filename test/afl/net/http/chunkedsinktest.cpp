/**
  *  \file test/afl/net/http/chunkedsinktest.cpp
  *  \brief Test for afl::net::http::ChunkedSink
  */

#include "afl/net/http/chunkedsink.hpp"

#include "afl/io/datasink.hpp"
#include "afl/string/string.hpp"
#include "afl/test/testrunner.hpp"

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

// Trivial case
AFL_TEST("afl.net.http.ChunkedSink:empty", a)
{
    TestSink t;
    afl::net::http::ChunkedSink cs(t);
    afl::base::ConstBytes_t data = afl::string::toBytes(TRIVIAL);
    a.check("handleData complete", cs.handleData(data));
    a.check("data processed", data.empty());
    a.checkEqual("result", t.m_text, "");
}

// Not so trivial
AFL_TEST("afl.net.http.ChunkedSink:simple", a)
{
    TestSink t;
    afl::net::http::ChunkedSink cs(t);
    afl::base::ConstBytes_t data = afl::string::toBytes(SIMPLE);
    a.check("handleData complete", cs.handleData(data));
    a.check("data processed", data.empty());
    a.checkEqual("result", t.m_text, "abcde");
}

// Data remains
AFL_TEST("afl.net.http.ChunkedSink:data-remains", a)
{
    TestSink t;
    afl::net::http::ChunkedSink cs(t);
    afl::base::ConstBytes_t data = afl::string::toBytes(REMAIN);
    a.check("handleData complete", cs.handleData(data));
    a.check("data processed incompletely", !data.empty());
    a.checkEqual("result", t.m_text, "abcde");
    a.checkEqual("remaining size", data.size(), 5U);
    a.checkEqualContent("remaining data", data, afl::string::toBytes("other"));
}

// Two parts
AFL_TEST("afl.net.http.ChunkedSink:two-parts", a)
{
    TestSink t;
    afl::net::http::ChunkedSink cs(t);
    afl::base::ConstBytes_t data = afl::string::toBytes(TWO_PARTS);
    a.check("handleData complete", cs.handleData(data));
    a.check("data processed", data.empty());
    a.checkEqual("result", t.m_text, "abcdefgh");
}

// Extension
AFL_TEST("afl.net.http.ChunkedSink:extension", a)
{
    TestSink t;
    afl::net::http::ChunkedSink cs(t);
    afl::base::ConstBytes_t data = afl::string::toBytes(EXTENSION);
    a.check("handleData complete", cs.handleData(data));
    a.check("data processed", data.empty());
    a.checkEqual("result", t.m_text, "abcdefgh");
}

// Incomplete - Two parts
AFL_TEST("afl.net.http.ChunkedSink:two-parts:incomplete", a)
{
    afl::base::ConstBytes_t data = afl::string::toBytes(TWO_PARTS);
    for (size_t i = 0; i < data.size(); ++i) {
        TestSink t;
        afl::net::http::ChunkedSink cs(t);
        afl::base::ConstBytes_t part(data.subrange(0, i));
        a.check("handleData incomplete", !cs.handleData(part));
        a.check("first part processed", part.empty());
        part = data.subrange(i);
        a.check("handleData complete", cs.handleData(part));
        a.check("second part processed", part.empty());
        a.checkEqual("result", t.m_text, "abcdefgh");
    }
}

// Incomplete - Extension
AFL_TEST("afl.net.http.ChunkedSink:two-parts:extension", a)
{
    afl::base::ConstBytes_t data = afl::string::toBytes(EXTENSION);
    for (size_t i = 0; i < data.size(); ++i) {
        TestSink t;
        afl::net::http::ChunkedSink cs(t);
        afl::base::ConstBytes_t part(data.subrange(0, i));
        a.check("handleData incomplete", !cs.handleData(part));
        a.check("first part processed", part.empty());
        part = data.subrange(i);
        a.check("handleData complete", cs.handleData(part));
        a.check("second part processed", part.empty());
        a.checkEqual("result", t.m_text, "abcdefgh");
    }
}
