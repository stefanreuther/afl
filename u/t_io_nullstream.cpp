/**
  *  \file u/t_io_nullstream.cpp
  *  \brief Test for afl::io::NullStream
  */

#include "afl/io/nullstream.hpp"

#include "u/t_io.hpp"
#include "afl/except/filetooshortexception.hpp"

/** Simple test. */
void
TestIoNullStream::testIt()
{
    afl::io::NullStream ns;
    TS_ASSERT_EQUALS(ns.getPos(), 0U);
    TS_ASSERT_EQUALS(ns.getSize(), 0U);

    // Write nothing. This will change the size!
    ns.setPos(99);
    TS_ASSERT_EQUALS(ns.getPos(), 99U);
    TS_ASSERT_EQUALS(ns.getSize(), 0U);

    size_t n = ns.write(afl::io::Stream::ConstBytes_t());
    TS_ASSERT_EQUALS(n, 0U);
    TS_ASSERT_EQUALS(ns.getPos(), 99U);
    TS_ASSERT_EQUALS(ns.getSize(), 99U);

    // Write something.
    ns.setPos(200);
    TS_ASSERT_EQUALS(ns.getPos(), 200U);
    TS_ASSERT_EQUALS(ns.getSize(), 99U);

    uint8_t data[3] = { 0, 1, 2 };
    n = ns.write(afl::io::Stream::ConstBytes_t(data));
    TS_ASSERT_EQUALS(n, 3U);
    TS_ASSERT_EQUALS(ns.getPos(), 203U);
    TS_ASSERT_EQUALS(ns.getSize(), 203U);

    // Write something at front
    ns.setPos(42);
    TS_ASSERT_EQUALS(ns.getPos(), 42U);
    TS_ASSERT_EQUALS(ns.getSize(), 203U);

    n = ns.write(afl::io::Stream::ConstBytes_t(data));
    TS_ASSERT_EQUALS(n, 3U);
    TS_ASSERT_EQUALS(ns.getPos(), 45U);
    TS_ASSERT_EQUALS(ns.getSize(), 203U);

    // Read
    TS_ASSERT_EQUALS(ns.read(afl::io::Stream::Bytes_t(data)), 0U);
    TS_ASSERT_EQUALS(ns.getPos(), 45U);
    TS_ASSERT_EQUALS(ns.getSize(), 203U);

    TS_ASSERT_THROWS(ns.fullRead(afl::io::Stream::Bytes_t(data)), afl::except::FileTooShortException);
}
