/**
  *  \file test/afl/io/internalsinktest.cpp
  *  \brief Test for afl::io::InternalSink
  */

#include "afl/io/internalsink.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.io.InternalSink", a)
{
    afl::io::InternalSink sink;
    a.check("initial content", sink.getContent().empty());

    uint8_t data[] = {1,2,3};
    afl::base::ConstBytes_t data1(data);
    a.check("data submitted", !sink.handleData(data1));
    a.checkEqual("content size", sink.getContent().size(), 3U);
    a.check("content", sink.getContent().equalContent(data));
}
