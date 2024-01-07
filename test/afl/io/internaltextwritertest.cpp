/**
  *  \file test/afl/io/internaltextwritertest.cpp
  *  \brief Test for afl::io::InternalTextWriter
  */

#include "afl/io/internaltextwriter.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.io.InternalTextWriter", a)
{
    // Initialisation: empty
    afl::io::InternalTextWriter testee;
    a.check("initially empty", testee.getContent().empty());

    // Append stuff
    testee.writeText("hi");
    testee.writeText("ho");
    testee.writeLine();
    testee.writeLine("ha");

    // Verify
    a.checkEqual("content after write", afl::string::fromMemory(testee.getContent()), "hiho\nha\n");

    // Clear
    testee.clear();
    a.check("empty after clear", testee.getContent().empty());

    // No flush required (but does not hurt)
    testee.writeText("hu");
    a.checkEqual("content after write 2", afl::string::fromMemory(testee.getContent()), "hu");
    AFL_CHECK_SUCCEEDS(a("flush"), testee.flush());
    a.checkEqual("content after write 3", afl::string::fromMemory(testee.getContent()), "hu");
}

