/**
  *  \file u/t_base_uncopyable.cpp
  *  \brief Test for afl::base::Uncopyable
  */

#include "afl/base/uncopyable.hpp"
#include "u/t_base.hpp"

/** Simple tests. */
void
TestBaseUncopyable::testIt()
{
    /* We cannot test much more than that the header file is wellformed,
       i.e. allows us to derive a class that can be successfully created. */
    class Tester : public afl::base::Uncopyable { };
    Tester t;
}
