/**
  *  \file u/t_io_constmemorystream.cpp
  *  \brief Test for afl::io::ConstMemoryStream
  */

#include "afl/io/constmemorystream.hpp"

#include "u/t_io.hpp"
#include "afl/except/fileproblemexception.hpp"

/** Simple test. */
void
TestIoConstMemoryStream::testIt()
{
    const uint8_t data[4] = { 1, 2, 3, 4 };
    afl::io::ConstMemoryStream mem(data);

    // Check basic properties
    TS_ASSERT_EQUALS(mem.getSize(), 4U);
    TS_ASSERT_EQUALS(mem.getPos(), 0U);
    TS_ASSERT(mem.hasCapabilities(mem.CanRead | mem.CanSeek));
    TS_ASSERT(!mem.hasCapabilities(mem.CanWrite));

    // Can read
    uint8_t r[3];
    size_t n = mem.read(r);
    TS_ASSERT_EQUALS(n, 3U);
    TS_ASSERT_EQUALS(mem.getPos(), 3U);
    TS_ASSERT_EQUALS(r[0], 1U);
    TS_ASSERT_EQUALS(r[1], 2U);
    TS_ASSERT_EQUALS(r[2], 3U);

    n = mem.read(r);
    TS_ASSERT_EQUALS(n, 1U);
    TS_ASSERT_EQUALS(mem.getPos(), 4U);
    TS_ASSERT_EQUALS(r[0], 4U);

    n = mem.read(r);
    TS_ASSERT_EQUALS(n, 0U);
    TS_ASSERT_EQUALS(mem.getPos(), 4U);

    // After end
    mem.setPos(99);
    TS_ASSERT_EQUALS(mem.getPos(), 99U);
    n = mem.read(r);
    TS_ASSERT_EQUALS(n, 0U);

    // Try fullRead at end
    mem.setPos(3);
    TS_ASSERT_THROWS(mem.fullRead(r), afl::except::FileProblemException);
}
