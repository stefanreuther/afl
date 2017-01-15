/**
  *  \file u/t_base_observable.cpp
  *  \brief Test for afl::base::Observable
  */

#include "afl/base/observable.hpp"

#include "t_base.hpp"

void
TestBaseObservable::testIt()
{
    // A listener
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

    // First test
    {
        // Default construct
        afl::base::Observable<int> testee;
        TS_ASSERT_EQUALS(testee.get(), 0);

        int counter = 0;
        testee.sig_change.addNewClosure(new Listener(counter));
        TS_ASSERT_EQUALS(counter, 0);

        testee.set(42);
        TS_ASSERT_EQUALS(counter, 1);

        testee.set(10);
        TS_ASSERT_EQUALS(counter, 2);

        testee.set(10);
        TS_ASSERT_EQUALS(counter, 2);
    }

    // Second test
    {
        // Construct from value
        afl::base::Observable<int> testee(42);
        TS_ASSERT_EQUALS(testee.get(), 42);

        int counter = 0;
        testee.sig_change.addNewClosure(new Listener(counter));
        TS_ASSERT_EQUALS(counter, 0);

        testee.set(42);
        TS_ASSERT_EQUALS(counter, 0);

        testee.set(10);
        TS_ASSERT_EQUALS(counter, 1);
    }
}

