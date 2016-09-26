/**
  *  \file u/t_io_internalstream.cpp
  *  \brief Test for afl::io::InternalStream
  */

#include "afl/io/internalstream.hpp"

#include <stdexcept>
#include "u/t_io.hpp"
#include "afl/except/fileproblemexception.hpp"

/** Simple test. */
void
TestIoInternalStream::testIt()
{
    afl::io::InternalStream mem;

    // Check basic properties
    TS_ASSERT_EQUALS(mem.getSize(), 0U);
    TS_ASSERT_EQUALS(mem.getPos(), 0U);
    TS_ASSERT(mem.hasCapabilities(mem.CanRead | mem.CanWrite | mem.CanSeek));
    TS_ASSERT_EQUALS(mem.getContent().size(), 0U);

    // Cannot read yet
    uint8_t r[3];
    size_t n = mem.read(r);
    TS_ASSERT_EQUALS(n, 0U);

    // After end
    mem.setPos(99);
    TS_ASSERT_EQUALS(mem.getPos(), 99U);
    n = mem.read(r);
    TS_ASSERT_EQUALS(n, 0U);

    // Write at beginning
    const uint8_t w[2] = { 88, 77 };
    mem.setPos(1);
    n = mem.write(w);
    TS_ASSERT_EQUALS(n, 2U);
    TS_ASSERT_EQUALS(mem.getPos(), 3U);
    TS_ASSERT_EQUALS(mem.getSize(), 3U);

    // Test content access
    afl::io::Stream::ConstBytes_t content(mem.getContent());
    TS_ASSERT_EQUALS(content.size(), 3U);
    TS_ASSERT_EQUALS(*content.eat(), 0U);
    TS_ASSERT_EQUALS(*content.eat(), 88U);
    TS_ASSERT_EQUALS(*content.eat(), 77U);
    TS_ASSERT_EQUALS(content.eat(), static_cast<const uint8_t*>(0));

    // Read again; must use just-set pointer
    mem.setPos(0);
    n = mem.read(r);
    TS_ASSERT_EQUALS(n, 3U);
    TS_ASSERT_EQUALS(mem.getPos(), 3U);
    TS_ASSERT_EQUALS(r[0], 0U);
    TS_ASSERT_EQUALS(r[1], 88U);
    TS_ASSERT_EQUALS(r[2], 77U);

    // Try fullRead at end
    mem.setPos(2);
    TS_ASSERT_THROWS(mem.fullRead(r), afl::except::FileProblemException);
    TS_ASSERT_EQUALS(mem.getPos(), 3U);
}

/** Test overflowing size. */
void
TestIoInternalStream::testOverflow()
{
    afl::io::InternalStream mem;
    mem.setPos(size_t(-1));
    TS_ASSERT_EQUALS(mem.getPos(), size_t(-1));
    TS_ASSERT_EQUALS(mem.getSize(), 0U);

    // Attempting to write 5 bytes will overflow a size_t.
    // (Or it will exhaust memory and throw a bad_alloc.)
    uint8_t data[5] = { 1,2,3,4,5 };
    TS_ASSERT_THROWS(mem.write(data), std::exception);
}

/** Test zero-byte boundary case. */
void
TestIoInternalStream::testZero()
{
    afl::io::InternalStream mem;
    TS_ASSERT_EQUALS(mem.write(afl::io::InternalStream::ConstBytes_t()), 0U);
}

/** Test name. */
void
TestIoInternalStream::testName()
{
    afl::io::InternalStream testee;
    TS_ASSERT_EQUALS(testee.getName(), "<internal>");

    testee.setName("hi");
    TS_ASSERT_EQUALS(testee.getName(), "hi");
}
