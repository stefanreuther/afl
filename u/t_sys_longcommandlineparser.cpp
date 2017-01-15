/**
  *  \file u/t_sys_longcommandlineparser.cpp
  */

#include "afl/sys/longcommandlineparser.hpp"

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

/** Test long options. */
void
TestSysLongCommandLineParser::testLong()
{
    static const char*const args[] = { "--help", "--foo", "bar", "--a=b", "--x=", "--y" };
    afl::sys::LongCommandLineParser p(*new Tester(args));

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

/** Test long options with single dash. */
void
TestSysLongCommandLineParser::testLong2()
{
    static const char*const args[] = { "-help", "-foo", "bar", "-a=b", "-x=", "-y" };
    afl::sys::LongCommandLineParser p(*new Tester(args));

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
TestSysLongCommandLineParser::testNonOption()
{
    static const char*const args[] = { "", "-", "a", "--", "-a", "a", "--", "b" };
    afl::sys::LongCommandLineParser p(*new Tester(args));

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
