/**
  *  \file u/t_io_internalfilemapping.cpp
  *  \brief Test for afl::io::InternalFileMapping
  */

#include "afl/io/internalfilemapping.hpp"

#include "u/t_io.hpp"
#include "afl/io/internalstream.hpp"
#include "afl/string/string.hpp"

/** Simple test. */
void
TestIoInternalFileMapping::testIt()
{
    // Create a stream
    afl::io::InternalStream stream;
    for (size_t i = 0; i < 1000; ++i) {
        stream.write(afl::string::toBytes("hi there"));
    }
    TS_ASSERT_EQUALS(stream.getPos(), 8000U);
    stream.setPos(3);

    // Create a mapping
    afl::io::InternalFileMapping map(stream, 200);
    TS_ASSERT_EQUALS(stream.getPos(), 203U);
    TS_ASSERT_EQUALS(map.get().size(), 200U);
    TS_ASSERT_EQUALS(*map.get().at(0), 't');
    TS_ASSERT_EQUALS(*map.get().at(1), 'h');
    TS_ASSERT_EQUALS(*map.get().at(2), 'e');
}

/** Test creation from Memory instance. */
void
TestIoInternalFileMapping::testMem()
{
    // Preload the buffer
    afl::base::GrowableBytes_t in;
    for (uint8_t i = 1; i <= 10; ++i) {
        in.append(i);
    }

    // Create
    afl::io::InternalFileMapping testee(in);
    afl::base::ConstBytes_t out(testee.get());
    TS_ASSERT_EQUALS(out.size(), 10U);
    for (uint8_t i = 1; i <= 10; ++i) {
        TS_ASSERT_EQUALS(*out.at(i-1), i);
    }
}
