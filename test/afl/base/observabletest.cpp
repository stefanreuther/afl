/**
  *  \file test/afl/base/observabletest.cpp
  *  \brief Test for afl::base::Observable
  */

#include "afl/base/observable.hpp"
#include "afl/test/testrunner.hpp"

namespace {
    class Listener : public afl::base::Closure<void()> {
     public:
        Listener(int& counter)
            : m_counter(counter)
            { }
        virtual void call()
            { ++m_counter; }
        virtual Listener* clone() const
            { return new Listener(m_counter); }
     private:
        int& m_counter;
    };
}

AFL_TEST("afl.base.Observable:1", a) {
    // Default construct
    afl::base::Observable<int> testee;
    a.checkEqual("initial value", testee.get(), 0);

    int counter = 0;
    testee.sig_change.addNewClosure(new Listener(counter));
    a.checkEqual("after registration", counter, 0);

    testee.set(42);
    a.checkEqual("after set", counter, 1);

    testee.set(10);
    a.checkEqual("after set different", counter, 2);

    testee.set(10);
    a.checkEqual("after set same", counter, 2);
}

AFL_TEST("afl.base.Observable:2", a) {
    // Construct from value
    afl::base::Observable<int> testee(42);
    a.checkEqual("initial value", testee.get(), 42);

    int counter = 0;
    testee.sig_change.addNewClosure(new Listener(counter));
    a.checkEqual("after registration", counter, 0);

    testee.set(42);
    a.checkEqual("after set same", counter, 0);

    testee.set(10);
    a.checkEqual("after set different", counter, 1);
}
