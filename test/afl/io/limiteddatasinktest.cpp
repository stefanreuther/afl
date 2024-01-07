/**
  *  \file test/afl/io/limiteddatasinktest.cpp
  *  \brief Test for afl::io::LimitedDataSink
  */

#include "afl/io/limiteddatasink.hpp"

#include "afl/io/internalsink.hpp"
#include "afl/test/testrunner.hpp"

// Zero
AFL_TEST("afl.io.LimitedDataSink:zero-empty-data", a)
{
    afl::io::InternalSink t;
    afl::io::LimitedDataSink lim(t, 0);
    afl::base::ConstBytes_t data;
    a.check("handleData", lim.handleData(data));
}

// Zero, data given
AFL_TEST("afl.io.LimitedDataSink:zero-data", a)
{
    afl::io::InternalSink t;
    afl::io::LimitedDataSink lim(t, 0);
    afl::base::ConstBytes_t data = afl::string::toBytes("abc");
    a.check("handleData", lim.handleData(data));
    a.checkEqual("size", data.size(), 3U);
}

// Some data given
AFL_TEST("afl.io.LimitedDataSink:nonzero", a)
{
    afl::io::InternalSink t;
    afl::io::LimitedDataSink lim(t, 10);
    afl::base::ConstBytes_t data = afl::string::toBytes("abc");
    a.check("handleData", !lim.handleData(data));
    a.checkEqual("size", data.size(), 0U);

    data = afl::string::toBytes("abcdefghi");
    a.check("handleData 2", lim.handleData(data));
    a.checkEqual("size 2", data.size(), 2U);
}
