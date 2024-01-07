/**
  *  \file test/afl/net/line/linehandlertest.cpp
  *  \brief Test for afl::net::line::LineHandler
  */

#include "afl/net/line/linehandler.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST_NOARG("afl.net.line.LineHandler")
{
    class Tester : public afl::net::line::LineHandler {
     public:
        virtual bool handleOpening(afl::net::line::LineSink& /*response*/)
            { return false; }
        virtual bool handleLine(const String_t& /*line*/, afl::net::line::LineSink& /*response*/)
            { return false; }
        virtual void handleConnectionClose()
            { }
    };
    Tester t;
}
