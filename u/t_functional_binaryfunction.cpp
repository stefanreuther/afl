/**
  *  \file u/t_functional_binaryfunction.cpp
  *  \brief Test for afl::functional::BinaryFunction
  */

#include "afl/functional/binaryfunction.hpp"

#include "u/t_functional.hpp"
#include "afl/string/string.hpp"
#include "afl/string/format.hpp"
#include "afl/functional/unaryfunction.hpp"

/** Simple tests for general usage and map(). */
void
TestFunctionalBinaryFunction::testIt()
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
    class Adder : public afl::functional::BinaryFunction<int,int,int> {
     public:
        int get(int a, int b) const
            { return a+b; }
    };

    // Test them alone
    TS_ASSERT_EQUALS(Increment()(10), 11);
    TS_ASSERT_EQUALS(Formatter()(10), "10");
    TS_ASSERT_EQUALS(Adder()(10, 20), 30);

    // Test composition
    TS_ASSERT_EQUALS(Adder().map(Formatter())(10, 23), "33");
    TS_ASSERT_EQUALS(Adder().map(Increment()).map(Formatter())(47, 11), "59");
}
