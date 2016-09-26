/**
  *  \file u/t_net_headertable.cpp
  *  \brief Test for afl::net::HeaderTable
  */

#include "afl/net/headertable.hpp"

#include "u/t_net.hpp"
#include "afl/io/internalsink.hpp"

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

/** Simple tests. */
void
TestNetHeaderTable::testIt()
{
    {
        afl::net::HeaderTable ht;
        TS_ASSERT_EQUALS(EnumTester(ht).toString(), "");
        ht.add("foo", "bar");
        TS_ASSERT_EQUALS(EnumTester(ht).toString(), "foo|bar");
        ht.add("foo", "bar2");
        TS_ASSERT_EQUALS(EnumTester(ht).toString(), "foo|bar#foo|bar2");
        ht.add("other", "bar");
        TS_ASSERT_EQUALS(EnumTester(ht).toString(), "foo|bar#foo|bar2#other|bar");

        afl::net::HeaderField* f = ht.get("foo");
        TS_ASSERT(f != 0);
        TS_ASSERT_EQUALS(f->getName(), "foo");
        TS_ASSERT_EQUALS(f->getValue(), "bar");

        f = ht.get("OTHER");
        TS_ASSERT(f != 0);
        TS_ASSERT_EQUALS(f->getName(), "other");
        TS_ASSERT_EQUALS(f->getValue(), "bar");

        TS_ASSERT(ht.get("bar") == 0);

        ht.set("foo", "baz");
        ht.set("x", "y");
        TS_ASSERT_EQUALS(EnumTester(ht).toString(), "foo|baz#foo|bar2#other|bar#x|y");

        // Write to sink and check
        afl::io::InternalSink sink;
        ht.writeHeaders("<test>", sink);
        TS_ASSERT(sink.getContent().equalContent(afl::string::toMemory("foo: baz\r\n"
                                                                       "foo: bar2\r\n"
                                                                       "other: bar\r\n"
                                                                       "x: y\r\n").toBytes()));
    }
}
