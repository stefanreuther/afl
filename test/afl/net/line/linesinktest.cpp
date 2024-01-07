/**
  *  \file test/afl/net/line/linesinktest.cpp
  *  \brief Test for afl::net::line::LineSink
  */

#include "afl/net/line/linesink.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST_NOARG("afl.net.line.LineSink")
{
    class Tester : public afl::net::line::LineSink {
     public:
        virtual void handleLine(const String_t& /*line*/)
            { }
    };
    Tester t;
}
