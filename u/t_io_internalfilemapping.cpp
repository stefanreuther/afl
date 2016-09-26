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
