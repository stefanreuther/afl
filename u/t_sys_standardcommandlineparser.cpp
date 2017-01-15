/**
  *  \file u/t_sys_standardcommandlineparser.cpp
  */

#include "afl/sys/standardcommandlineparser.hpp"

#include "u/t_sys.hpp"
#include "afl/base/memory.hpp"

namespace {
    class Tester : public afl::sys::Environment::CommandLine_t {
     public:
        Tester(afl::base::Memory<const char*const> argv)
            : m_argv(argv)
            { }

        bool getNextElement(String_t& result)
            {
                if (const char*const* p = m_argv.eat()) {
                    result = *p;
                    return true;
                } else {
                    return false;
                }
            }

     private:
        afl::base::Memory<const char*const> m_argv;
    };
}

/** Test character options. */
void
TestSysStandardCommandLineParser::testChars()
{
    static const char*const args[] = { "-a", "-abc", "-xfoo", "-x", "foo", "non", "-", "-y", "--", "-y" };
    afl::sys::StandardCommandLineParser p(*new Tester(args));

    String_t value;
    bool opt;

    // "-a"
    TS_ASSERT(p.getNext(opt, value));
    TS_ASSERT(opt);
    TS_ASSERT_EQUALS(value, "a");
    TS_ASSERT(p.getFlags().empty());

    // "-abc"
    TS_ASSERT(p.getNext(opt, value));
    TS_ASSERT(opt);
    TS_ASSERT_EQUALS(value, "a");
    TS_ASSERT(p.getFlags().empty());

    TS_ASSERT(p.getNext(opt, value));
    TS_ASSERT(opt);
    TS_ASSERT_EQUALS(value, "b");
    TS_ASSERT(p.getFlags().empty());

    TS_ASSERT(p.getNext(opt, value));
    TS_ASSERT(opt);
    TS_ASSERT_EQUALS(value, "c");
    TS_ASSERT(p.getFlags().empty());

    // "-xfoo"
    TS_ASSERT(p.getNext(opt, value));
    TS_ASSERT(opt);
    TS_ASSERT_EQUALS(value, "x");
    TS_ASSERT(p.getParameter(value));
    TS_ASSERT_EQUALS(value, "foo");
    TS_ASSERT(p.getFlags().empty());

    // "-x foo"
    TS_ASSERT(p.getNext(opt, value));
    TS_ASSERT(opt);
    TS_ASSERT_EQUALS(value, "x");
    TS_ASSERT(p.getParameter(value));
    TS_ASSERT_EQUALS(value, "foo");
    TS_ASSERT(p.getFlags().empty());

    // "non"
    TS_ASSERT(p.getNext(opt, value));
    TS_ASSERT(!opt);
    TS_ASSERT_EQUALS(value, "non");
    TS_ASSERT(p.getFlags().empty());

    // "-"
    TS_ASSERT(p.getNext(opt, value));
    TS_ASSERT(!opt);
    TS_ASSERT_EQUALS(value, "-");
    TS_ASSERT(p.getFlags().empty());

    // "-y"
    TS_ASSERT(p.getNext(opt, value));
    TS_ASSERT(opt);
    TS_ASSERT_EQUALS(value, "y");
    TS_ASSERT(p.getFlags().empty());

    // "-- -y"
    TS_ASSERT(p.getNext(opt, value));
    TS_ASSERT(!opt);
    TS_ASSERT_EQUALS(value, "-y");
    TS_ASSERT(p.getFlags().empty());

    // End
    TS_ASSERT(!p.getNext(opt, value));
    TS_ASSERT(!p.getNext(opt, value));
    TS_ASSERT(!p.getNext(opt, value));
    TS_ASSERT(p.getFlags().empty());
}

/** Test long options. */
void
TestSysStandardCommandLineParser::testLong()
{
    static const char*const args[] = { "--help", "--foo", "bar", "--a=b", "--x=", "--y" };
    afl::sys::StandardCommandLineParser p(*new Tester(args));

    String_t value;
    bool opt;

    // "--help"
    TS_ASSERT(p.getNext(opt, value));
    TS_ASSERT(opt);
    TS_ASSERT_EQUALS(value, "help");
    TS_ASSERT(p.getFlags().contains(p.IsLongOption));
    TS_ASSERT(!p.getFlags().contains(p.HasParameter));

    // "--foo bar"
    TS_ASSERT(p.getNext(opt, value));
    TS_ASSERT(opt);
    TS_ASSERT_EQUALS(value, "foo");
    TS_ASSERT(p.getFlags().contains(p.IsLongOption));
    TS_ASSERT(!p.getFlags().contains(p.HasParameter));
    TS_ASSERT(p.getParameter(value));
    TS_ASSERT_EQUALS(value, "bar");

    // "--a=b"
    TS_ASSERT(p.getNext(opt, value));
    TS_ASSERT(opt);
    TS_ASSERT_EQUALS(value, "a");
    TS_ASSERT(p.getFlags().contains(p.IsLongOption));
    TS_ASSERT(p.getFlags().contains(p.HasParameter));
    TS_ASSERT(p.getParameter(value));
    TS_ASSERT_EQUALS(value, "b");

    // "--x="
    TS_ASSERT(p.getNext(opt, value));
    TS_ASSERT(opt);
    TS_ASSERT_EQUALS(value, "x");
    TS_ASSERT(p.getFlags().contains(p.IsLongOption));
    TS_ASSERT(p.getFlags().contains(p.HasParameter));
    TS_ASSERT(p.getParameter(value));
    TS_ASSERT_EQUALS(value, "");

    // "--y"
    TS_ASSERT(p.getNext(opt, value));
    TS_ASSERT(opt);
    TS_ASSERT_EQUALS(value, "y");
    TS_ASSERT(p.getFlags().contains(p.IsLongOption));
    TS_ASSERT(!p.getFlags().contains(p.HasParameter));
    TS_ASSERT(!p.getParameter(value));

    TS_ASSERT(!p.getNext(opt, value));
}

/** Test nonoptions. */
void
TestSysStandardCommandLineParser::testNonOption()
{
    static const char*const args[] = { "", "-", "a", "--", "-a", "a", "--", "b" };
    afl::sys::StandardCommandLineParser p(*new Tester(args));

    String_t value;
    bool opt;

    // ""
    TS_ASSERT(p.getNext(opt, value));
    TS_ASSERT(!opt);
    TS_ASSERT_EQUALS(value, "");
    TS_ASSERT(p.getFlags().empty());

    // "-"
    TS_ASSERT(p.getNext(opt, value));
    TS_ASSERT(!opt);
    TS_ASSERT_EQUALS(value, "-");
    TS_ASSERT(p.getFlags().empty());

    // "a"
    TS_ASSERT(p.getNext(opt, value));
    TS_ASSERT(!opt);
    TS_ASSERT_EQUALS(value, "a");
    TS_ASSERT(p.getFlags().empty());

    // "-- -a"
    TS_ASSERT(p.getNext(opt, value));
    TS_ASSERT(!opt);
    TS_ASSERT_EQUALS(value, "-a");
    TS_ASSERT(p.getFlags().empty());

    // "a"
    TS_ASSERT(p.getNext(opt, value));
    TS_ASSERT(!opt);
    TS_ASSERT_EQUALS(value, "a");
    TS_ASSERT(p.getFlags().empty());

    // "--"
    TS_ASSERT(p.getNext(opt, value));
    TS_ASSERT(!opt);
    TS_ASSERT_EQUALS(value, "--");
    TS_ASSERT(p.getFlags().empty());

    // "b"
    TS_ASSERT(p.getNext(opt, value));
    TS_ASSERT(!opt);
    TS_ASSERT_EQUALS(value, "b");
    TS_ASSERT(p.getFlags().empty());

    TS_ASSERT(!p.getNext(opt, value));
}
