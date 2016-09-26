/**
  *  \file u/t_io_directoryentry.cpp
  *  \brief Test for afl::io::DirectoryEntry
  */

#include "afl/io/directoryentry.hpp"

#include "u/t_io.hpp"
#include "afl/base/ptr.hpp"

/** Simple test. */
void
TestIoDirectoryEntry::testIt()
{
    // No further tests; just make sure it compiles.
    afl::base::Ptr<afl::io::DirectoryEntry> entry;
}
