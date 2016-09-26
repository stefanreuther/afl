/**
  *  \file u/t_functional_mapping.cpp
  */

#include "afl/functional/mapping.hpp"

#include "u/t_functional.hpp"

void
TestFunctionalMapping::testIt()
{
    // Test functions
    class Adder : public afl::functional::BinaryFunction<int,int,int> {
     public:
        int get(int a, int b) const
            { return a+b; }
    };
    class Squares : public afl::functional::Mapping<int,int> {
     public:
        bool getFirstKey(int& a) const {
            a = 1;
            return true;
        }
        bool getNextKey(int& a) const {
            if (a < 10) {
                ++a;
                return true;
            } else {
                return false;
            }
        }
        int get(int x) const
            { return x*x; }
    };
    class Increment : public afl::functional::UnaryFunction<int,int> {
     public:
        int get(int a) const
            { return a+1; }
    };

    // Test them alone
    TS_ASSERT_EQUALS(Increment()(10), 11);
    TS_ASSERT_EQUALS(Adder()(4, 9), 13);
    TS_ASSERT_EQUALS(Squares()(3), 9);

    int i;
    TS_ASSERT(Squares().getFirstKey(i));
    TS_ASSERT_EQUALS(i, 1);
    TS_ASSERT(Squares().getNextKey(i));
    TS_ASSERT_EQUALS(i, 2);
    i = 9;
    TS_ASSERT(Squares().getNextKey(i));
    TS_ASSERT_EQUALS(i, 10);
    TS_ASSERT(!Squares().getNextKey(i));

    // Test combinations
    // - sum_{i=1..10} i*i
    TS_ASSERT_EQUALS(Squares().fold(Adder(), 0), 385);

    // - sum_{i=1..10} (i*i+1)
    TS_ASSERT_EQUALS(Squares().map(Increment()).fold(Adder(), 0), 395);
}
