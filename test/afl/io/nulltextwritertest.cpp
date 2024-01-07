/**
  *  \file test/afl/io/nulltextwritertest.cpp
  *  \brief Test for afl::io::NullTextWriter
  */

#include "afl/io/nulltextwriter.hpp"
#include "afl/test/testrunner.hpp"

/** Simple test.
    This class has no behaviour, but it must be creatible. */
AFL_TEST_NOARG("afl.io.NullTextWriter")
{
    afl::io::NullTextWriter testee;

    // Call some methods, for coverage
    testee.writeText("hello world\n");
    testee.writeText(String_t("hello world\n"));
    testee.writeText(afl::string::toMemory("hello world\n"));
    testee.flush();
}
