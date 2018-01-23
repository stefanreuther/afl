/**
  *  \file u/t_base_stoppable.cpp
  *  \brief Test for afl::base::Stoppable
  */

#include "afl/base/stoppable.hpp"

#include "t_base.hpp"

/** Interface test. */
void
TestBaseStoppable::testInterface()
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

