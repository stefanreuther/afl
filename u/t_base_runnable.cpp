/**
  *  \file u/t_base_runnable.cpp
  *  \brief Test for afl::base::Runnable
  */

#include "afl/base/runnable.hpp"
#include "u/t_base.hpp"

/** Simple test. */
void
TestBaseRunnable::testIt()
{
    /* Test that we can create a runnable in the way we mean to.
       That's pretty much the only thing we can test, but so it
       verifies that our header file is syntactically valid. */
    class Tester : public afl::base::Runnable {
     public:
        void run()
            {
                TS_ASSERT(true);
            }
    };

    Tester t;
    t.run();
    TS_ASSERT(true);
}

