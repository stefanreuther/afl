/**
  *  \file u/t_io_memorystream.cpp
  *  \brief Test for afl::io::MemoryStream
  */

#include "afl/io/memorystream.hpp"

#include "u/t_io.hpp"
#include "afl/except/fileproblemexception.hpp"

/** Simple test. */
void
TestIoMemoryStream::testIt()
{
    uint8_t data[4] = { 1, 2, 3, 4 };
    afl::io::MemoryStream mem(data);

    // Check basic properties
    TS_ASSERT_EQUALS(mem.getSize(), 4U);
    TS_ASSERT_EQUALS(mem.getPos(), 0U);
    TS_ASSERT(mem.hasCapabilities(mem.CanRead | mem.CanWrite | mem.CanSeek));

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

    // Write after end
    n = mem.write(r);
    TS_ASSERT_EQUALS(n, 0U);

    // Write at beginning
    const uint8_t w[2] = { 88, 77 };
    mem.setPos(1);
    n = mem.write(w);
    TS_ASSERT_EQUALS(n, 2U);
    TS_ASSERT_EQUALS(mem.getPos(), 3U);
    TS_ASSERT_EQUALS(data[0], 1U);
    TS_ASSERT_EQUALS(data[1], 88U);
    TS_ASSERT_EQUALS(data[2], 77U);
    TS_ASSERT_EQUALS(data[3], 4U);

    // Read again; must use just-set pointer
    n = mem.read(r);
    TS_ASSERT_EQUALS(n, 1U);
    TS_ASSERT_EQUALS(mem.getPos(), 4U);

    // Read again; must use just-set data
    mem.setPos(1);
    n = mem.read(r);
    TS_ASSERT_EQUALS(n, 3U);
    TS_ASSERT_EQUALS(r[0], 88U);
    TS_ASSERT_EQUALS(r[1], 77U);
    TS_ASSERT_EQUALS(r[2], 4U);

    // Try fullRead at end
    mem.setPos(3);
    TS_ASSERT_THROWS(mem.fullRead(r), afl::except::FileProblemException);
    TS_ASSERT_EQUALS(mem.getPos(), 4U);
    TS_ASSERT_THROWS(mem.fullWrite(w), afl::except::FileProblemException);
}
