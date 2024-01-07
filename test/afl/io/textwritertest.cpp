/**
  *  \file test/afl/io/textwritertest.cpp
  *  \brief Test for afl::io::TextWriter
  */

#include "afl/io/textwriter.hpp"
#include "afl/test/testrunner.hpp"

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

AFL_TEST("afl.io.TextWriter:empty", a)
{
    Tester t;
    a.checkEqual("", t.getString(), "");
}

AFL_TEST("afl.io.TextWriter:writeText", a)
{
    Tester t;
    t.writeText("foo");
    t.writeText(String_t("bar"));
    a.checkEqual("", t.getString(), "foobar");
}

AFL_TEST("afl.io.TextWriter:writeText+writeLine", a)
{
    Tester t;
    t.writeText("foo");
    t.writeLine();
    t.writeText("bar");
    a.checkEqual("", t.getString(), "foo|bar");
}

AFL_TEST("afl.io.TextWriter:writeLine", a)
{
    Tester t;
    t.writeLine("foo");
    t.writeLine(String_t("bar"));
    a.checkEqual("", t.getString(), "foo|bar|");
}

AFL_TEST("afl.io.TextWriter:writeText:memory", a)
{
    // The ConstStringMemory_t signature will include the trailing null byte if constructed from a string!
    Tester t;
    t.writeText(afl::string::ConstStringMemory_t("foo"));
    a.checkEqual("", t.getString().size(), 4U);
}

AFL_TEST("afl.io.TextWriter:writeText:nl", a)
{
    Tester t;
    t.writeText("foo\n");
    a.checkEqual("", t.getString(), "foo|");
}

AFL_TEST("afl.io.TextWriter:writeText+writeLine:nl", a)
{
    Tester t;
    t.writeText("foo\n");
    t.writeLine();
    a.checkEqual("", t.getString(), "foo||");
}

AFL_TEST("afl.io.TextWriter:writeLine:nl", a)
{
    Tester t;
    t.writeLine("foo\nbar\n\nbaz");
    a.checkEqual("", t.getString(), "foo|bar||baz|");
}
