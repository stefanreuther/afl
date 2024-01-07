/**
  *  \file test/afl/base/weaktargettest.cpp
  *  \brief Test for afl::base::WeakTarget
  */

#include "afl/base/weaktarget.hpp"
#include "afl/test/testrunner.hpp"

/** Simple test.
    We cannot test much here,
    so we only test the implementor interface: it must be possible to create and instantiate a class derived from WeakTarget. */
AFL_TEST_NOARG("afl.base.WeakTarget")
{
    class Tester : public afl::base::WeakTarget { };
    Tester testee;
}
