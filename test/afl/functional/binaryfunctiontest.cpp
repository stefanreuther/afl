/**
  *  \file test/afl/functional/binaryfunctiontest.cpp
  *  \brief Test for afl::functional::BinaryFunction
  */

#include "afl/functional/binaryfunction.hpp"

#include "afl/functional/unaryfunction.hpp"
#include "afl/string/format.hpp"
#include "afl/string/string.hpp"
#include "afl/test/testrunner.hpp"

/** Simple tests for general usage and map(). */
AFL_TEST("afl.functional.BinaryFunction", a)
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
    a.checkEqual("Increment", Increment()(10), 11);
    a.checkEqual("Formatter", Formatter()(10), "10");
    a.checkEqual("Adder", Adder()(10, 20), 30);

    // Test composition
    a.checkEqual("Adder.Formatter", Adder().map(Formatter())(10, 23), "33");
    a.checkEqual("Adder.Increment.Formatter", Adder().map(Increment()).map(Formatter())(47, 11), "59");
}
