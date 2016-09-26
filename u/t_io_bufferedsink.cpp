/**
  *  \file u/t_io_bufferedsink.cpp
  *  \brief Test for afl::io::BufferedSink
  */

#include "afl/io/bufferedsink.hpp"

#include "u/t_io.hpp"
#include "afl/io/internalsink.hpp"

/** Simple test. */
void
TestIoBufferedSink::testIt()
{
    // Build it
    afl::io::InternalSink internal;
    afl::io::BufferedSink buffered(internal);

    // Write some data
    uint8_t data[] = {1,2,3};
    afl::base::ConstBytes_t desc(data);
    bool result = buffered.handleData("<source name>", desc);
    TS_ASSERT(!result);

    // Some more data
    uint8_t data1[4096];
    afl::base::Bytes_t(data1).fill(0);
    afl::base::ConstBytes_t desc1(data1);
    result = buffered.handleData("<source name>", desc1);
    TS_ASSERT(!result);

    // Even more
    desc = data;
    result = buffered.handleData("<source name>", desc);
    TS_ASSERT(!result);

    // Flush
    buffered.flush();

    // Verify
    TS_ASSERT_EQUALS(internal.getContent().size(), 4096u + 3u + 3u);

    afl::base::ConstBytes_t out(internal.getContent());
    TS_ASSERT_EQUALS(*out.eat(), 1U);
    TS_ASSERT_EQUALS(*out.eat(), 2U);
    TS_ASSERT_EQUALS(*out.eat(), 3U);
    TS_ASSERT_EQUALS(*out.eat(), 0U);
    TS_ASSERT_EQUALS(*out.eat(), 0U);
    TS_ASSERT_EQUALS(*out.eat(), 0U);
}
