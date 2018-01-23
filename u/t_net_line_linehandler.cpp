/**
  *  \file u/t_net_line_linehandler.cpp
  *  \brief Test for afl::net::line::LineHandler
  */

#include "afl/net/line/linehandler.hpp"

#include "t_net_line.hpp"

/** Interface test. */
void
TestNetLineLineHandler::testInterface()
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

