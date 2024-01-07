/**
  *  \file test/afl/base/deletabletest.cpp
  *  \brief Test for afl::base::Deletable
  */

#include "afl/base/deletable.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST_NOARG("afl.base.Deletable") {
    /* We cannot test much more than that the header file is wellformed,
       i.e. allows us to derive a class that can be successfully created. */
    class Tester : public afl::base::Deletable { };
    Tester t;
}
