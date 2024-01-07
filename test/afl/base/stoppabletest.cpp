/**
  *  \file test/afl/base/stoppabletest.cpp
  *  \brief Test for afl::base::Stoppable
  */

#include "afl/base/stoppable.hpp"
#include "afl/test/testrunner.hpp"

/** Interface test. */
AFL_TEST_NOARG("afl.base.Stoppable")
{
    class Tester : public afl::base::Stoppable {
     public:
        virtual void run()
            { }
        virtual void stop()
            { }
    };
    Tester t;
}
