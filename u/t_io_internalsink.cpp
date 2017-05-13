/**
  *  \file u/t_io_internalsink.cpp
  *  \brief Test for afl::io::InternalSink
  */

#include "afl/io/internalsink.hpp"

#include "u/t_io.hpp"

/** Simple test. */
void
TestIoInternalSink::testIt()
{
    afl::io::InternalSink sink;
    TS_ASSERT(sink.getContent().empty());

    uint8_t data[] = {1,2,3};
    afl::base::ConstBytes_t data1(data);
    TS_ASSERT(!sink.handleData(data1));
    TS_ASSERT_EQUALS(sink.getContent().size(), 3U);
    TS_ASSERT(sink.getContent().equalContent(data));
}
