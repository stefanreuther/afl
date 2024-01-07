/**
  *  \file test/afl/functional/mappingtest.cpp
  *  \brief Test for afl::functional::Mapping
  */

#include "afl/functional/mapping.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.functional.Mapping", a)
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
    a.checkEqual("Increment", Increment()(10), 11);
    a.checkEqual("Adder", Adder()(4, 9), 13);
    a.checkEqual("Squares", Squares()(3), 9);

    int i;
    a.check("01. getFirstKey", Squares().getFirstKey(i));
    a.checkEqual("02. value", i, 1);
    a.check("02. getNextKey", Squares().getNextKey(i));
    a.checkEqual("03. value", i, 2);
    i = 9;
    a.check("04. getNextKey", Squares().getNextKey(i));
    a.checkEqual("05. value", i, 10);
    a.check("06. getNextKey", !Squares().getNextKey(i));

    // Test combinations
    // - sum_{i=1..10} i*i
    a.checkEqual("fold", Squares().fold(Adder(), 0), 385);

    // - sum_{i=1..10} (i*i+1)
    a.checkEqual("map.fold", Squares().map(Increment()).fold(Adder(), 0), 395);
}
