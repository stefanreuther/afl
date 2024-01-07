/**
  *  \file test/afl/io/internalfilemappingtest.cpp
  *  \brief Test for afl::io::InternalFileMapping
  */

#include "afl/io/internalfilemapping.hpp"

#include "afl/test/testrunner.hpp"
#include "afl/io/internalstream.hpp"
#include "afl/string/string.hpp"

/** Simple test. */
AFL_TEST("afl.io.InternalFileMapping:from-stream", a)
{
    // Create a stream
    afl::io::InternalStream stream;
    for (size_t i = 0; i < 1000; ++i) {
        stream.write(afl::string::toBytes("hi there"));
    }
    a.checkEqual("stream getPos", stream.getPos(), 8000U);
    stream.setPos(3);

    // Create a mapping
    afl::io::InternalFileMapping map(stream, 200);
    a.checkEqual("getPos", stream.getPos(), 203U);
    a.checkEqual("size", map.get().size(), 200U);
    a.checkEqual("at 0", *map.get().at(0), 't');
    a.checkEqual("at 1", *map.get().at(1), 'h');
    a.checkEqual("at 2", *map.get().at(2), 'e');
}

/** Test creation from Memory instance. */
AFL_TEST("afl.io.InternalFileMapping:from-memory", a)
{
    // Preload the buffer
    afl::base::GrowableBytes_t in;
    for (uint8_t i = 1; i <= 10; ++i) {
        in.append(i);
    }

    // Create
    afl::io::InternalFileMapping testee(in);
    afl::base::ConstBytes_t out(testee.get());
    a.checkEqual("size", out.size(), 10U);
    for (uint8_t i = 1; i <= 10; ++i) {
        a.checkEqual("content", *out.at(i-1), i);
    }
}
