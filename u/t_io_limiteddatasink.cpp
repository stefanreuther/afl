/**
  *  \file u/t_io_limiteddatasink.cpp
  *  \brief Test for afl::io::LimitedDataSink
  */

#include "afl/io/limiteddatasink.hpp"

#include "u/t_io.hpp"
#include "afl/io/internalsink.hpp"

/** Simple test. */
void
TestIoLimitedDataSink::testIt()
{
    {
        // Zero
        afl::io::InternalSink t;
        afl::io::LimitedDataSink lim(t, 0);
        afl::base::ConstBytes_t data;
        TS_ASSERT(lim.handleData(data));
    }

    {
        // Zero, data given
        afl::io::InternalSink t;
        afl::io::LimitedDataSink lim(t, 0);
        afl::base::ConstBytes_t data = afl::string::toBytes("abc");
        TS_ASSERT(lim.handleData(data));
        TS_ASSERT_EQUALS(data.size(), 3U);
    }

    {
        // Some data given
        afl::io::InternalSink t;
        afl::io::LimitedDataSink lim(t, 10);
        afl::base::ConstBytes_t data = afl::string::toBytes("abc");
        TS_ASSERT(!lim.handleData(data));
        TS_ASSERT_EQUALS(data.size(), 0U);

        data = afl::string::toBytes("abcdefghi");
        TS_ASSERT(lim.handleData(data));
        TS_ASSERT_EQUALS(data.size(), 2U);
    }
}
