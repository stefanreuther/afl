/**
  *  \file u/t_base_deletable.cpp
  *  \brief Test for afl::base::Deletable
  */

#include "afl/base/deletable.hpp"

#include "u/t_base.hpp"

/** Simple tests. */
void
TestBaseDeletable::testIt()
{
    /* We cannot test much more than that the header file is wellformed,
       i.e. allows us to derive a class that can be successfully created. */
    class Tester : public afl::base::Deletable { };
    Tester t;
}
