/**
  *  \file u/t_io_internaltextwriter.cpp
  *  \brief Test for afl::io::InternalTextWriter
  */

#include "afl/io/internaltextwriter.hpp"

#include "t_io.hpp"

/** Simple sequence test. */
void
TestIoInternalTextWriter::testIt()
{
    // Initialisation: empty
    afl::io::InternalTextWriter testee;
    TS_ASSERT(testee.getContent().empty());

    // Append stuff
    testee.writeText("hi");
    testee.writeText("ho");
    testee.writeLine();
    testee.writeLine("ha");

    // Verify
    TS_ASSERT_EQUALS(afl::string::fromMemory(testee.getContent()), "hiho\nha\n");

    // Clear
    testee.clear();
    TS_ASSERT(testee.getContent().empty());

    // No flush required (but does not hurt)
    testee.writeText("hu");
    TS_ASSERT_EQUALS(afl::string::fromMemory(testee.getContent()), "hu");
    TS_ASSERT_THROWS_NOTHING(testee.flush());
    TS_ASSERT_EQUALS(afl::string::fromMemory(testee.getContent()), "hu");
}

