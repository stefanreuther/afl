/**
  *  \file test/afl/functional/unaryfunctiontest.cpp
  *  \brief Test for afl::functional::UnaryFunction
  */

#include "afl/functional/unaryfunction.hpp"

#include "afl/test/testrunner.hpp"
#include "afl/string/string.hpp"
#include "afl/string/format.hpp"
#include "afl/base/optional.hpp"
#include "afl/base/inlineoptional.hpp"

/** Simple tests for general usage and map(). */
AFL_TEST("afl.functional.UnaryFunction", a)
{
    // Test functions
    class Increment : public afl::functional::UnaryFunction<int,int> {
     public:
        int get(int a) const
            { return a+1; }
    };
    class Formatter : public afl::functional::UnaryFunction<int,String_t> {
     public:
        String_t get(int n) const
            { return afl::string::Format("%d", n); }
    };
    class Wrapper : public afl::functional::UnaryFunction<String_t,String_t> {
     public:
        String_t get(String_t x) const
            { return "(" + x + ")"; }
    };

    // Test them alone
    a.checkEqual("Increment", Increment()(10), 11);
    a.checkEqual("Formatter", Formatter()(10), "10");
    a.checkEqual("Wrapper", Wrapper()("xy"), "(xy)");

    // Test composition
    a.checkEqual("Increment.Formatter", Increment().map(Formatter())(10), "11");
    a.checkEqual("Increment.Formatter.Wrapper", Increment().map(Formatter()).map(Wrapper())(10), "(11)");
}

/** Test behaviour with optional values. */
AFL_TEST("afl.functional.UnaryFunction:optional", a)
{
    class Increment : public afl::functional::UnaryFunction<int,int> {
     public:
        int get(int a) const
            { return a+1; }
    };

    afl::base::Optional<int> oi, oj;
    a.check("01", !oi.isValid());

    oj = Increment()(oi);
    a.check("02", !oj.isValid());

    oj = Increment()(10);
    a.check("03", oj.isValid());
    a.checkEqual("04", *oj.get(), 11);

    oi = 7;
    oj = Increment()(oi);
    a.check("05", oj.isValid());
    a.checkEqual("06", *oj.get(), 8);

    oj = Increment()(Increment()(Increment()(oj)));
    a.check("07", oj.isValid());
    a.checkEqual("08", *oj.get(), 11);

    oj = Increment()(afl::base::InlineOptional<int,-1>(3));
    a.check("09", oj.isValid());
    a.checkEqual("10", *oj.get(), 4);

    oj = Increment()(afl::base::InlineOptional<int,-1>());
    a.check("11", !oj.isValid());

    oj = Increment()(afl::base::InlineOptional<char,-1>(42));
    a.check("12", oj.isValid());
    a.checkEqual("13", *oj.get(), 43);

    oj = Increment()(afl::base::InlineOptional<char,-1>());
    a.check("14", !oj.isValid());
}
