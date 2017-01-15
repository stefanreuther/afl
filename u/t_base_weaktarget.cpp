/**
  *  \file u/t_base_weaktarget.cpp
  *  \brief Test for afl::base::WeakTarget
  */

#include "afl/base/weaktarget.hpp"

#include "t_base.hpp"

/** Simple test.
    We cannot test much here,
    so we only test the implementor interface: it must be possible to create and instantiate a class derived from WeakTarget. */
void
TestBaseWeakTarget::testIt()
{
    class Tester : public afl::base::WeakTarget { };
    Tester testee;
}

