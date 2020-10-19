/**
  *  \file u/t_io_limitedstream.cpp
  *  \brief Test for afl::io::LimitedStream
  */

#include "afl/io/limitedstream.hpp"

#include "t_io.hpp"
#include "afl/io/internalstream.hpp"
#include "afl/except/fileproblemexception.hpp"

/** Test fully-functional view into file.
    FIXME: add more cases */
void
TestIoLimitedStream::testFull()
{
    // Create a stream containing 100 bytes
    afl::io::InternalStream is;
    for (uint8_t i = 0; i < 100; ++i) {
        is.write(afl::base::fromObject(i));
    }

    // Create a view for 10 bytes starting at 20
    afl::io::LimitedStream testee(is.createChild(), 20, 10);
    TS_ASSERT_EQUALS(testee.getPos(), 0U);

    // Read one byte
    uint8_t byte[1];
    TS_ASSERT_EQUALS(testee.read(byte), 1U);
    TS_ASSERT_EQUALS(byte[0], 20U);
    TS_ASSERT_EQUALS(testee.getPos(), 1U);

    // Go to end
    testee.setPos(9);
    TS_ASSERT_EQUALS(testee.read(byte), 1U);
    TS_ASSERT_EQUALS(byte[0], 29U);
    TS_ASSERT_EQUALS(testee.getPos(), 10U);

    TS_ASSERT_EQUALS(testee.read(byte), 0U);
    TS_ASSERT_EQUALS(testee.getPos(), 10U);

    // Out-of-bounds seek throws
    TS_ASSERT_THROWS_NOTHING(testee.setPos(0));
    TS_ASSERT_THROWS_NOTHING(testee.setPos(10));
    TS_ASSERT_THROWS(testee.setPos(11), afl::except::FileProblemException);
    TS_ASSERT_THROWS(testee.setPos(afl::io::Stream::FileSize_t(-1)), afl::except::FileProblemException);

    // Capabilities
    uint32_t cap = testee.getCapabilities();
    TS_ASSERT((cap & afl::io::Stream::CanRead) != 0);
    TS_ASSERT((cap & afl::io::Stream::CanWrite) != 0);
    TS_ASSERT((cap & afl::io::Stream::CanSeek) != 0);
}
