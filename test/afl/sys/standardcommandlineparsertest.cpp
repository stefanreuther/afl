/**
  *  \file test/afl/sys/standardcommandlineparsertest.cpp
  *  \brief Test for afl::sys::StandardCommandLineParser
  */

#include "afl/sys/standardcommandlineparser.hpp"

#include "afl/base/memory.hpp"
#include "afl/test/testrunner.hpp"

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
AFL_TEST("afl.sys.StandardCommandLineParser:char-options", a)
{
    static const char*const args[] = { "-a", "-abc", "-xfoo", "-x", "foo", "non", "-", "-y", "--", "-y" };
    afl::sys::StandardCommandLineParser p(*new Tester(args));

    String_t value;
    bool opt;

    // "-a"
    a.check("10. getNext", p.getNext(opt, value));
    a.check("02. opt", opt);
    a.checkEqual("03. value", value, "a");
    a.check("04. getFlags", p.getFlags().empty());

    // "-abc"
    a.check("11. getNext", p.getNext(opt, value));
    a.check("12. opt", opt);
    a.checkEqual("13. value", value, "a");
    a.check("14. getFlags", p.getFlags().empty());

    a.check("21. getNext", p.getNext(opt, value));
    a.check("22. opt", opt);
    a.checkEqual("23. value", value, "b");
    a.check("24. getFlags", p.getFlags().empty());

    a.check("31. getNext", p.getNext(opt, value));
    a.check("32. opt", opt);
    a.checkEqual("33. value", value, "c");
    a.check("34. getFlags", p.getFlags().empty());

    // "-xfoo"
    a.check("41. getNext", p.getNext(opt, value));
    a.check("42. opt", opt);
    a.checkEqual("43. value", value, "x");
    a.check("44. getParameter", p.getParameter(value));
    a.checkEqual("45. value", value, "foo");
    a.check("46. getFlags", p.getFlags().empty());

    // "-x foo"
    a.check("51. getNext", p.getNext(opt, value));
    a.check("52. opt", opt);
    a.checkEqual("53. value", value, "x");
    a.check("54. getParameter", p.getParameter(value));
    a.checkEqual("55. value", value, "foo");
    a.check("56. getFlags", p.getFlags().empty());

    // "non"
    a.check("61. getNext", p.getNext(opt, value));
    a.check("62. opt", !opt);
    a.checkEqual("63. value", value, "non");
    a.check("64. getFlags", p.getFlags().empty());

    // "-"
    a.check("71. getNext", p.getNext(opt, value));
    a.check("72. opt", !opt);
    a.checkEqual("73. value", value, "-");
    a.check("74. getFlags", p.getFlags().empty());

    // "-y"
    a.check("81. getNext", p.getNext(opt, value));
    a.check("82. opt", opt);
    a.checkEqual("83. value", value, "y");
    a.check("84. getFlags", p.getFlags().empty());

    // "-- -y"
    a.check("91. getNext", p.getNext(opt, value));
    a.check("92. opt", !opt);
    a.checkEqual("93. value", value, "-y");
    a.check("94. getFlags", p.getFlags().empty());

    // End
    a.check("101. getNext", !p.getNext(opt, value));
    a.check("102. getNext", !p.getNext(opt, value));
    a.check("103. getNext", !p.getNext(opt, value));
    a.check("104. getFlags", p.getFlags().empty());
}

/** Test long options. */
AFL_TEST("afl.sys.StandardCommandLineParser:long-options", a)
{
    static const char*const args[] = { "--help", "--foo", "bar", "--a=b", "--x=", "--y" };
    afl::sys::StandardCommandLineParser p(*new Tester(args));

    String_t value;
    bool opt;

    // "--help"
    a.check("01. getNext", p.getNext(opt, value));
    a.check("02. opt", opt);
    a.checkEqual("03. value", value, "help");
    a.check("04. getFlags", p.getFlags().contains(p.IsLongOption));
    a.check("05. getFlags", !p.getFlags().contains(p.HasParameter));

    // "--foo bar"
    a.check("11. getNext", p.getNext(opt, value));
    a.check("12. opt", opt);
    a.checkEqual("13. value", value, "foo");
    a.check("14. getFlags", p.getFlags().contains(p.IsLongOption));
    a.check("15. getFlags", !p.getFlags().contains(p.HasParameter));
    a.check("16. getParameter", p.getParameter(value));
    a.checkEqual("17. value", value, "bar");

    // "--a=b"
    a.check("21. getNext", p.getNext(opt, value));
    a.check("22. opt", opt);
    a.checkEqual("23. value", value, "a");
    a.check("24. getFlags", p.getFlags().contains(p.IsLongOption));
    a.check("25. getFlags", p.getFlags().contains(p.HasParameter));
    a.check("26. getParameter", p.getParameter(value));
    a.checkEqual("27. value", value, "b");

    // "--x="
    a.check("31. getNext", p.getNext(opt, value));
    a.check("32. opt", opt);
    a.checkEqual("33. value", value, "x");
    a.check("34. getFlags", p.getFlags().contains(p.IsLongOption));
    a.check("35. getFlags", p.getFlags().contains(p.HasParameter));
    a.check("36. getParameter", p.getParameter(value));
    a.checkEqual("37. value", value, "");

    // "--y"
    a.check("41. getNext", p.getNext(opt, value));
    a.check("42. opt", opt);
    a.checkEqual("43. value", value, "y");
    a.check("44. getFlags", p.getFlags().contains(p.IsLongOption));
    a.check("45. getFlags", !p.getFlags().contains(p.HasParameter));
    a.check("46. getParameter", !p.getParameter(value));

    a.check("99. getNext", !p.getNext(opt, value));
}

/** Test nonoptions. */
AFL_TEST("afl.sys.StandardCommandLineParser:non-options", a)
{
    static const char*const args[] = { "", "-", "a", "--", "-a", "a", "--", "b" };
    afl::sys::StandardCommandLineParser p(*new Tester(args));

    String_t value;
    bool opt;

    // ""
    a.check("01. getNext", p.getNext(opt, value));
    a.check("02. opt", !opt);
    a.checkEqual("03. value", value, "");
    a.check("04. getFlags", p.getFlags().empty());

    // "-"
    a.check("11. getNext", p.getNext(opt, value));
    a.check("12. opt", !opt);
    a.checkEqual("13. value", value, "-");
    a.check("14. getFlags", p.getFlags().empty());

    // "a"
    a.check("21. getNext", p.getNext(opt, value));
    a.check("22. opt", !opt);
    a.checkEqual("23. value", value, "a");
    a.check("24. getFlags", p.getFlags().empty());

    // "-- -a"
    a.check("31. getNext", p.getNext(opt, value));
    a.check("32. opt", !opt);
    a.checkEqual("33. value", value, "-a");
    a.check("34. getFlags", p.getFlags().empty());

    // "a"
    a.check("41. getNext", p.getNext(opt, value));
    a.check("42. opt", !opt);
    a.checkEqual("43. value", value, "a");
    a.check("44. getFlags", p.getFlags().empty());

    // "--"
    a.check("51. getNext", p.getNext(opt, value));
    a.check("52. opt", !opt);
    a.checkEqual("53. value", value, "--");
    a.check("54. getFlags", p.getFlags().empty());

    // "b"
    a.check("61. getNext", p.getNext(opt, value));
    a.check("62. opt", !opt);
    a.checkEqual("63. value", value, "b");
    a.check("64. getFlags", p.getFlags().empty());

    a.check("99. getNext", !p.getNext(opt, value));
}
