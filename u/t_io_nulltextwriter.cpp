/**
  *  \file u/t_io_nulltextwriter.cpp
  *  \brief Test for afl::io::NullTextWriter
  */

#include "afl/io/nulltextwriter.hpp"

#include "u/t_io.hpp"

/** Simple test.
    This class has no behaviour, but it must be creatible. */
void
TestIoNullTextWriter::testIt()
{
    afl::io::NullTextWriter testee;

    // Call some methods, for coverage
    testee.writeText("hello world\n");
    testee.writeText(String_t("hello world\n"));
    testee.writeText(afl::string::toMemory("hello world\n"));
    testee.flush();
}
