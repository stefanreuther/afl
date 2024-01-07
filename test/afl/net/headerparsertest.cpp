/**
  *  \file test/afl/net/headerparsertest.cpp
  *  \brief Test for afl::net::HeaderParser
  */

#include "afl/net/headerparser.hpp"

#include "afl/test/testrunner.hpp"
#include "afl/net/headerconsumer.hpp"
#include "afl/base/memory.hpp"

namespace {
    class Consumer : public afl::net::HeaderConsumer {
     public:
        void handleHeader(String_t key, String_t value)
            {
                if (!m_buffer.empty()) {
                    m_buffer += "||";
                }
                m_buffer += key;
                m_buffer += "==";
                m_buffer += value;
            }

        String_t m_buffer;

        /*
         *  Test utilities
         */

        /** Test a valid header.
            \param dataChars data
            \param expectedResult expected result, in a==b||c==d format
            \param folding expected status of hasFolding() */
        void testValid(afl::test::Assert a, afl::base::Memory<const char> dataChars, const char* expectedResult, bool folding)
            {
                dataChars.trim(dataChars.size() - 1);
                afl::base::ConstBytes_t bytes(dataChars.toBytes());

                afl::net::HeaderParser p(*this);
                a.check("handleData", p.handleData(bytes));
                a.check("bytes empty", bytes.empty());
                a.check("no errors", !p.hasErrors());
                a.check("isCompleted", p.isCompleted());
                a.checkEqual("hasFolding", p.hasFolding(), folding);
                a.checkEqual("result", m_buffer, expectedResult);
            }

        /** Test an incomplete header.
            \param dataChars data */
        void testIncomplete(afl::test::Assert a, afl::base::Memory<const char> dataChars)
            {
                dataChars.trim(dataChars.size() - 1);
                afl::base::ConstBytes_t bytes(dataChars.toBytes());

                afl::net::HeaderParser p(*this);
                a.check("handleData", !p.handleData(bytes));
                a.check("isCompleted", !p.isCompleted());
                a.check("bytes empty", bytes.empty());
            }

        /** Test an invalid header.
            \param dataChars data
            \param expectedResult expected result, in a==b||c==d format */
        void testInvalid(afl::test::Assert a, afl::base::Memory<const char> dataChars, const char* expectedResult)
            {
                dataChars.trim(dataChars.size() - 1);
                afl::base::ConstBytes_t bytes(dataChars.toBytes());

                afl::net::HeaderParser p(*this);
                a.check("handleData", p.handleData(bytes));
                a.check("bytes empty", bytes.empty());
                a.check("no errors", p.hasErrors());
                a.check("isCompleted", p.isCompleted());
                a.checkEqual("result", m_buffer, expectedResult);
            }
    };
}

/** Test parsing of split data.
    No matter at what point we split the data, result must be identical. */
AFL_TEST("afl.net.HeaderParser:partitioned-data", a)
{
    afl::base::Memory<const char> dataChars("First: first-value\r\n"
                                            "Second: second-value\r\n"
                                            "Third:\r\n"
                                            " third-value\r\n"
                                            "Fourth:\r\n"
                                            "\r\n");

    static const char result[] = "First==first-value||Second==second-value||Third==third-value||Fourth==";

    // Remove trailing '\0'
    dataChars.trim(dataChars.size() - 1);

    // Initial sanity check
    {
        Consumer c;
        afl::net::HeaderParser p(c);
        afl::base::ConstBytes_t mem(dataChars.toBytes());
        a.check("01. handleData", p.handleData(mem));
        a.check("02. complete", mem.empty());
        a.checkEqual("03. result", c.m_buffer, result);
    }

    // Test all partitions
    for (size_t i = 0; i < dataChars.size(); ++i) {
        Consumer c;
        afl::net::HeaderParser p(c);

        afl::base::ConstBytes_t mem1(dataChars.subrange(0, i).toBytes());
        afl::base::ConstBytes_t mem2(dataChars.subrange(i).toBytes());
        a.check("01. handleData", !p.handleData(mem1));
        a.check("02. complete", mem1.empty());
        a.check("03. handleData", p.handleData(mem2));
        a.check("04. complete", mem2.empty());
        a.checkEqual("05. result", c.m_buffer, result);
    }
}

/** Test valid headers. */
AFL_TEST("afl.net.HeaderParser:valid", a)
{
    Consumer().testValid(a("one header, no space"), "First:first\n\n", "First==first", false);
    Consumer().testValid(a("one header, one space"), "First: first\n\n", "First==first", false);
    Consumer().testValid(a("one header, many spaces"), "First:                   first\n\n", "First==first", false);
    Consumer().testValid(a("one header, continuation"), "First: \n first\n\n", "First==first", true);
    Consumer().testValid(a("three headers"),
                         "First: first\n"
                         "Second: second\n"
                         "Third: third\n\n", "First==first||Second==second||Third==third", false);
    Consumer().testValid(a("repeated header"),
                         "Repeated: 1\n"
                         "Repeated: 2\n"
                         "Repeated: 3\n\n", "Repeated==1||Repeated==2||Repeated==3", false);
    Consumer().testValid(a("folded data"), "Folded: a\n b\n c\n\n", "Folded==a b c", true);
    Consumer().testValid(a("null header"), "\n", "", false);
}

/** Test invalid/incomplete headers. */
AFL_TEST("afl.net.HeaderParser:invalid", a)
{
    Consumer().testIncomplete(a("missing nl"),    "First: first");
    Consumer().testIncomplete(a("missing nl+nl"), "First: first\n");

    Consumer().testInvalid(a("missing name"),  " \n\n", "");
    Consumer().testInvalid(a("indented name"), " Foo:bar\n\n", "");
    Consumer().testInvalid(a("missing colon"), "First=first\n\n", "");
    Consumer().testInvalid(a("leading colon"), "a:b\n:c\n\n", "a==b");
}
