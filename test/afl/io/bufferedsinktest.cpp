/**
  *  \file test/afl/io/bufferedsinktest.cpp
  *  \brief Test for afl::io::BufferedSink
  */

#include "afl/io/bufferedsink.hpp"

#include "afl/io/internalsink.hpp"
#include "afl/test/testrunner.hpp"

namespace {
    class BrokenSink : public afl::io::DataSink {
     public:
        virtual bool handleData(afl::base::ConstBytes_t& /*data*/)
            { throw std::runtime_error("boom"); }
    };
}

/** Simple test. */
AFL_TEST("afl.io.BufferedSink", a)
{
    // Build it
    afl::io::InternalSink internal;
    afl::io::BufferedSink buffered(internal);

    // Write some data
    uint8_t data[] = {1,2,3};
    afl::base::ConstBytes_t desc(data);
    bool result = buffered.handleData(desc);
    a.check("handleData result", !result);

    // Some more data
    uint8_t data1[4096];
    afl::base::Bytes_t(data1).fill(0);
    afl::base::ConstBytes_t desc1(data1);
    result = buffered.handleData(desc1);
    a.check("handleData result 2", !result);

    // Even more
    desc = data;
    result = buffered.handleData(desc);
    a.check("handleData result 3", !result);

    // Flush
    buffered.flush();

    // Verify
    a.checkEqual("getContent", internal.getContent().size(), 4096u + 3u + 3u);

    afl::base::ConstBytes_t out(internal.getContent());
    a.checkEqual("byte 1", *out.eat(), 1U);
    a.checkEqual("byte 2", *out.eat(), 2U);
    a.checkEqual("byte 3", *out.eat(), 3U);
    a.checkEqual("byte 4", *out.eat(), 0U);
    a.checkEqual("byte 5", *out.eat(), 0U);
    a.checkEqual("byte 6", *out.eat(), 0U);
}

/** Test failure to write.
    Write into the buffer must not report an error, flush must.
    Unsuccessful flush must reset the state to allow successful destruction of the object. */
AFL_TEST("afl.io.BufferedSink:backend-failure:flush", a)
{
    // Create a BufferedSink whose underlying sink reports an error
    BrokenSink sink;
    afl::io::BufferedSink testee(sink);

    // First write does not report an error
    uint8_t data[1] = {0};
    AFL_CHECK_SUCCEEDS(a("handleFullData"), testee.handleFullData(data));

    // This reports the error
    AFL_CHECK_THROWS(a("flush"), testee.flush(), std::exception);

    // At this point, the BrokenSink must be in a state that it can be successfully destroyed.
}

/** Test failure to write.
    Write into the buffer must not report an error.
    In this test case, the error is reported in the destructor,
    which will cause std::terminate being called if the BufferedSink does not guard against it. */
AFL_TEST("afl.io.BufferedSink:backend-failure:no-flush", a)
{
    try {
        // Create a BufferedSink whose underlying sink reports an error
        BrokenSink sink;
        afl::io::BufferedSink testee(sink);

        // First write does not report an error
        uint8_t data[1] = {0};
        AFL_CHECK_SUCCEEDS(a("handleFullData"), testee.handleFullData(data));

        // At this point, the BrokenSink must be in a state that it can be successfully destroyed.
        throw int(99);
    }
    catch (int) {
    }
}

/** Test failure to write.
    Flush due to buffer overrun must reset the state and not leave the buffer in a filled state. */
AFL_TEST_NOARG("afl.io.BufferedSink:backend-failure:implicit")
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
