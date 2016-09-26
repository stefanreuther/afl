/**
  *  \file u/t_functional_unaryfunction.cpp
  *  \brief Test for afl::functional::UnaryFunction
  */

#include "afl/functional/unaryfunction.hpp"

#include "u/t_functional.hpp"
#include "afl/string/string.hpp"
#include "afl/string/format.hpp"
#include "afl/base/optional.hpp"
#include "afl/base/inlineoptional.hpp"

/** Simple tests for general usage and map(). */
void
TestFunctionalUnaryFunction::testIt()
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
    TS_ASSERT_EQUALS(Increment()(10), 11);
    TS_ASSERT_EQUALS(Formatter()(10), "10");
    TS_ASSERT_EQUALS(Wrapper()("xy"), "(xy)");

    // Test composition
    TS_ASSERT_EQUALS(Increment().map(Formatter())(10), "11");
    TS_ASSERT_EQUALS(Increment().map(Formatter()).map(Wrapper())(10), "(11)");
}

/** Test behaviour with optional values. */
void
TestFunctionalUnaryFunction::testOptional()
{
    class Increment : public afl::functional::UnaryFunction<int,int> {
     public:
        int get(int a) const
            { return a+1; }
    };

    afl::base::Optional<int> oi, oj;
    TS_ASSERT(!oi.isValid());

    oj = Increment()(oi);
    TS_ASSERT(!oj.isValid());

    oj = Increment()(10);
    TS_ASSERT(oj.isValid());
    TS_ASSERT_EQUALS(*oj.get(), 11);

    oi = 7;
    oj = Increment()(oi);
    TS_ASSERT(oj.isValid());
    TS_ASSERT_EQUALS(*oj.get(), 8);

    oj = Increment()(Increment()(Increment()(oj)));
    TS_ASSERT(oj.isValid());
    TS_ASSERT_EQUALS(*oj.get(), 11);

    oj = Increment()(afl::base::InlineOptional<int,-1>(3));
    TS_ASSERT(oj.isValid());
    TS_ASSERT_EQUALS(*oj.get(), 4);

    oj = Increment()(afl::base::InlineOptional<int,-1>());
    TS_ASSERT(!oj.isValid());

    oj = Increment()(afl::base::InlineOptional<char,-1>(42));
    TS_ASSERT(oj.isValid());
    TS_ASSERT_EQUALS(*oj.get(), 43);

    oj = Increment()(afl::base::InlineOptional<char,-1>());
    TS_ASSERT(!oj.isValid());
}
