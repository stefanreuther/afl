/**
  *  \file test/afl/base/uncopyabletest.cpp
  *  \brief Test for afl::base::Uncopyable
  */

#include "afl/base/uncopyable.hpp"
#include "afl/test/testrunner.hpp"

/** Simple tests. */
AFL_TEST_NOARG("afl.base.Uncopyable") {
    /* We cannot test much more than that the header file is wellformed,
       i.e. allows us to derive a class that can be successfully created. */
    class Tester : public afl::base::Uncopyable { };
    Tester t;
}
