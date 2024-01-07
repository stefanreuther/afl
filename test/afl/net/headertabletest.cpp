/**
  *  \file test/afl/net/headertabletest.cpp
  *  \brief Test for afl::net::HeaderTable
  */

#include "afl/net/headertable.hpp"

#include "afl/io/internalsink.hpp"
#include "afl/test/testrunner.hpp"

namespace {
    class EnumTester : public afl::net::HeaderConsumer {
     public:
        EnumTester(afl::net::HeaderTable& tbl)
            { tbl.enumerateHeaders(*this); }

        const String_t& toString() const
            { return m_string; }

        void handleHeader(String_t name, String_t value)
            {
                if (!m_string.empty()) {
                    m_string += "#";
                }
                m_string += name;
                m_string += "|";
                m_string += value;
            }

     private:
        String_t m_string;
    };
}

AFL_TEST("afl.net.HeaderTable", a)
{
    afl::net::HeaderTable ht;
    a.checkEqual("01. toString", EnumTester(ht).toString(), "");
    ht.add("foo", "bar");
    a.checkEqual("02. toString", EnumTester(ht).toString(), "foo|bar");
    ht.add("foo", "bar2");
    a.checkEqual("03. toString", EnumTester(ht).toString(), "foo|bar#foo|bar2");
    ht.add("other", "bar");
    a.checkEqual("04. toString", EnumTester(ht).toString(), "foo|bar#foo|bar2#other|bar");

    afl::net::HeaderField* f = ht.get("foo");
    a.checkNonNull("11. get", f);
    a.checkEqual("12. name", f->getName(), "foo");
    a.checkEqual("13. value", f->getValue(), "bar");

    f = ht.get("OTHER");
    a.checkNonNull("21. get", f);
    a.checkEqual("22. name", f->getName(), "other");
    a.checkEqual("23. value", f->getValue(), "bar");

    a.checkNull("31. get", ht.get("bar"));

    ht.set("foo", "baz");
    ht.set("x", "y");
    a.checkEqual("41. toString", EnumTester(ht).toString(), "foo|baz#foo|bar2#other|bar#x|y");

    // Write to sink and check
    afl::io::InternalSink sink;
    ht.writeHeaders(sink);
    a.check("51. writeHeaders", sink.getContent().equalContent(afl::string::toMemory("foo: baz\r\n"
                                                                                     "foo: bar2\r\n"
                                                                                     "other: bar\r\n"
                                                                                     "x: y\r\n").toBytes()));
}
