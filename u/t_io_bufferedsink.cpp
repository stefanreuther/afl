/**
  *  \file u/t_io_bufferedsink.cpp
  *  \brief Test for afl::io::BufferedSink
  */

#include <stdexcept>
#include "afl/io/bufferedsink.hpp"

#include "u/t_io.hpp"
#include "afl/io/internalsink.hpp"

namespace {
    class BrokenSink : public afl::io::DataSink {
     public:
        virtual bool handleData(afl::base::ConstBytes_t& /*data*/)
            { throw std::runtime_error("boom"); }
    };
}

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
    bool result = buffered.handleData(desc);
    TS_ASSERT(!result);

    // Some more data
    uint8_t data1[4096];
    afl::base::Bytes_t(data1).fill(0);
    afl::base::ConstBytes_t desc1(data1);
    result = buffered.handleData(desc1);
    TS_ASSERT(!result);

    // Even more
    desc = data;
    result = buffered.handleData(desc);
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

/** Test failure to write.
    Write into the buffer must not report an error, flush must.
    Unsuccessful flush must reset the state to allow successful destruction of the object. */
void
TestIoBufferedSink::testFail()
{
    // Create a BufferedSink whose underlying sink reports an error
    BrokenSink sink;
    afl::io::BufferedSink testee(sink);

    // First write does not report an error
    uint8_t data[1] = {0};
    TS_ASSERT_THROWS_NOTHING(testee.handleFullData(data));

    // This reports the error
    TS_ASSERT_THROWS(testee.flush(), std::exception);

    // At this point, the BrokenSink must be in a state that it can be successfully destroyed.
}

/** Test failure to write.
    Write into the buffer must not report an error.
    In this test case, the error is reported in the destructor,
    which will cause std::terminate being called if the BufferedSink does not guard against it. */
void
TestIoBufferedSink::testFail2()
{
    try {
        // Create a BufferedSink whose underlying sink reports an error
        BrokenSink sink;
        afl::io::BufferedSink testee(sink);

        // First write does not report an error
        uint8_t data[1] = {0};
        TS_ASSERT_THROWS_NOTHING(testee.handleFullData(data));

        // At this point, the BrokenSink must be in a state that it can be successfully destroyed.
        throw int(99);
    }
    catch (int) {
    }
}

/** Test failure to write.
    Flush due to buffer overrun must reset the state and not leave the buffer in a filled state. */
void
TestIoBufferedSink::testFail3()
{
    // Create a BufferedSink whose underlying sink reports an error
    BrokenSink sink;
    afl::io::BufferedSink testee(sink);

    // Fill the BufferedSink
    uint8_t data[128];
    afl::base::Bytes_t(data).fill(0);
    for (int i = 0; i < 1000; ++i) {
        try {
            testee.handleFullData(data);
        }
        catch (std::exception&) {
            // ignore
        }
    }

    try {
        testee.flush();
    }
    catch (std::exception&) {
        // ignore
    }

    // At this point, the BrokenSink must be in a state that it can be successfully destroyed.
}

