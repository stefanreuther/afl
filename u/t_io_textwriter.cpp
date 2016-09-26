/**
  *  \file u/t_io_textwriter.cpp
  *  \brief Test for afl::io::TextWriter
  */

#include "afl/io/textwriter.hpp"

#include "u/t_io.hpp"

namespace {
    class Tester : public afl::io::TextWriter {
     public:
        Tester()
            : m_string()
            { }

        const String_t& getString() const
            { return m_string; }

     protected:
        virtual void doWriteText(afl::string::ConstStringMemory_t data)
            { m_string += afl::string::fromMemory(data); }
        virtual void doWriteNewline()
            { m_string += '|'; }
        virtual void doFlush()
            { }

     private:
        String_t m_string;
    };
}

/** Interface test. */
void
TestIoTextWriter::testIt()
{
    {
        Tester t;
        TS_ASSERT_EQUALS(t.getString(), "");
    }
    {
        // writeText
        Tester t;
        t.writeText("foo");
        t.writeText(String_t("bar"));
        TS_ASSERT_EQUALS(t.getString(), "foobar");
    }
    {
        // writeText, writeLine
        Tester t;
        t.writeText("foo");
        t.writeLine();
        t.writeText("bar");
        TS_ASSERT_EQUALS(t.getString(), "foo|bar");
    }
    {
        // writeLine
        Tester t;
        t.writeLine("foo");
        t.writeLine(String_t("bar"));
        TS_ASSERT_EQUALS(t.getString(), "foo|bar|");
    }
    {
        // The ConstStringMemory_t signature will include the trailing null byte if constructed from a string!
        Tester t;
        t.writeText(afl::string::ConstStringMemory_t("foo"));
        TS_ASSERT_EQUALS(t.getString().size(), 4U);
    }
}

/** Test behaviour with newlines. */
void
TestIoTextWriter::testNewline()
{
    {
        // writeText
        Tester t;
        t.writeText("foo\n");
        TS_ASSERT_EQUALS(t.getString(), "foo|");
    }
    {
        // writeText, writeLine
        Tester t;
        t.writeText("foo\n");
        t.writeLine();
        TS_ASSERT_EQUALS(t.getString(), "foo||");
    }
    {
        // writeLine
        Tester t;
        t.writeLine("foo\nbar\n\nbaz");
        TS_ASSERT_EQUALS(t.getString(), "foo|bar||baz|");
    }
}
