/**
  *  \file u/t_net_headerparser.cpp
  *  \brief Test for afl::net::HeaderParser
  */

#include "afl/net/headerparser.hpp"

#include "u/t_net.hpp"
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
        void testValid(afl::base::Memory<const char> dataChars, const char* expectedResult, bool folding)
            {
                dataChars.trim(dataChars.size() - 1);
                afl::base::ConstBytes_t bytes(dataChars.toBytes());

                afl::net::HeaderParser p(*this);
                TSM_ASSERT(expectedResult, p.handleData("<source name>", bytes));
                TSM_ASSERT(expectedResult, bytes.empty());
                TSM_ASSERT(expectedResult, !p.hasErrors());
                TSM_ASSERT(expectedResult, p.isCompleted());
                TSM_ASSERT_EQUALS(expectedResult, p.hasFolding(), folding);
                TSM_ASSERT_EQUALS(expectedResult, m_buffer, expectedResult);
            }

        /** Test an incomplete header.
            \param dataChars data */
        void testIncomplete(afl::base::Memory<const char> dataChars)
            {
                dataChars.trim(dataChars.size() - 1);
                afl::base::ConstBytes_t bytes(dataChars.toBytes());

                afl::net::HeaderParser p(*this);
                TS_ASSERT(!p.handleData("<source name>", bytes));
                TS_ASSERT(!p.isCompleted());
                TS_ASSERT(bytes.empty());
            }

        /** Test an invalid header.
            \param dataChars data
            \param expectedResult expected result, in a==b||c==d format */
        void testInvalid(afl::base::Memory<const char> dataChars, const char* expectedResult)
            {
                dataChars.trim(dataChars.size() - 1);
                afl::base::ConstBytes_t bytes(dataChars.toBytes());

                afl::net::HeaderParser p(*this);
                TSM_ASSERT(expectedResult, p.handleData("<source name>", bytes));
                TSM_ASSERT(expectedResult, bytes.empty());
                TSM_ASSERT(expectedResult, p.hasErrors());
                TSM_ASSERT(expectedResult, p.isCompleted());
                TSM_ASSERT_EQUALS(expectedResult, m_buffer, expectedResult);
            }
    };
}

/** Test parsing of split data. */
void
TestNetHeaderParser::testPartition()
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
        TS_ASSERT(p.handleData("<source name>", mem));
        TS_ASSERT(mem.empty());
        TS_ASSERT_EQUALS(c.m_buffer, result);
    }

    // Test all partitions
    for (size_t i = 0; i < dataChars.size(); ++i) {
        Consumer c;
        afl::net::HeaderParser p(c);

        afl::base::ConstBytes_t mem1(dataChars.subrange(0, i).toBytes());
        afl::base::ConstBytes_t mem2(dataChars.subrange(i).toBytes());
        TS_ASSERT(!p.handleData("<source name>", mem1));
        TS_ASSERT(mem1.empty());
        TS_ASSERT(p.handleData("<source name>", mem2));
        TS_ASSERT(mem2.empty());
        TS_ASSERT_EQUALS(c.m_buffer, result);
    }
}

/** Test valid headers. */
void
TestNetHeaderParser::testValid()
{
    Consumer().testValid("First:first\n\n", "First==first", false);
    Consumer().testValid("First: first\n\n", "First==first", false);
    Consumer().testValid("First:                   first\n\n", "First==first", false);
    Consumer().testValid("First: \n first\n\n", "First==first", true);
    Consumer().testValid("First: first\n"
                         "Second: second\n"
                         "Third: third\n\n", "First==first||Second==second||Third==third", false);
    Consumer().testValid("Repeated: 1\n"
                         "Repeated: 2\n"
                         "Repeated: 3\n\n", "Repeated==1||Repeated==2||Repeated==3", false);
    Consumer().testValid("Folded: a\n b\n c\n\n", "Folded==a b c", true);
    Consumer().testValid("\n", "", false);
}

/** Test invalid/incomplete headers. */
void
TestNetHeaderParser::testInvalid()
{
    Consumer().testIncomplete("First: first");
    Consumer().testIncomplete("First: first\n");

    Consumer().testInvalid(" \n\n", "");
    Consumer().testInvalid(" Foo:bar\n\n", "");
    Consumer().testInvalid("First=first\n\n", "");
    Consumer().testInvalid("a:b\n:c\n\n", "a==b");
}
