/**
  *  \file test/afl/base/runnabletest.cpp
  *  \brief Test for afl::base::Runnable
  */

#include "afl/base/runnable.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST_NOARG("afl.base.Runnable")
{
    /* Test that we can create a runnable in the way we mean to.
       That's pretty much the only thing we can test, but so it
       verifies that our header file is syntactically valid. */
    class Tester : public afl::base::Runnable {
     public:
        void run()
            { }
    };

    Tester t;
    t.run();
}
