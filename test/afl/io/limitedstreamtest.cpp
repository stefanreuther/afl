/**
  *  \file test/afl/io/limitedstreamtest.cpp
  *  \brief Test for afl::io::LimitedStream
  */

#include "afl/io/limitedstream.hpp"

#include "afl/except/fileproblemexception.hpp"
#include "afl/io/internalstream.hpp"
#include "afl/test/testrunner.hpp"

/** Test fully-functional view into file.
    FIXME: add more cases */
AFL_TEST("afl.io.LimitedStream", a)
{
    // Create a stream containing 100 bytes
    afl::io::InternalStream is;
    for (uint8_t i = 0; i < 100; ++i) {
        is.write(afl::base::fromObject(i));
    }

    // Create a view for 10 bytes starting at 20
    afl::io::LimitedStream testee(is.createChild(0), 20, 10);
    a.checkEqual("01. getPos", testee.getPos(), 0U);

    // Read one byte
    uint8_t byte[1];
    a.checkEqual("02. read", testee.read(byte), 1U);
    a.checkEqual("03. data", byte[0], 20U);
    a.checkEqual("04. getPos", testee.getPos(), 1U);

    // Go to end
    testee.setPos(9);
    a.checkEqual("11. read", testee.read(byte), 1U);
    a.checkEqual("12. data", byte[0], 29U);
    a.checkEqual("13. getPos", testee.getPos(), 10U);

    a.checkEqual("14. read", testee.read(byte), 0U);
    a.checkEqual("15. getPos", testee.getPos(), 10U);

    // Out-of-bounds seek throws
    AFL_CHECK_SUCCEEDS(a("21. setPos"), testee.setPos(0));
    AFL_CHECK_SUCCEEDS(a("22. setPos"), testee.setPos(10));
    AFL_CHECK_THROWS(a("23. setPos"), testee.setPos(11), afl::except::FileProblemException);
    AFL_CHECK_THROWS(a("24. setPos"), testee.setPos(afl::io::Stream::FileSize_t(-1)), afl::except::FileProblemException);

    // Capabilities
    uint32_t cap = testee.getCapabilities();
    a.check("31. getCapabilities", (cap & afl::io::Stream::CanRead) != 0);
    a.check("32. getCapabilities", (cap & afl::io::Stream::CanWrite) != 0);
    a.check("33. getCapabilities", (cap & afl::io::Stream::CanSeek) != 0);
}
