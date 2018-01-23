/**
  *  \file u/t_net_line_linesink.cpp
  *  \brief Test for afl::net::line::LineSink
  */

#include "afl/net/line/linesink.hpp"

#include "t_net_line.hpp"

/** Interface test. */
void
TestNetLineLineSink::testInterface()
{
    class Tester : public afl::net::line::LineSink {
     public:
        virtual void handleLine(const String_t& /*line*/)
            { }
    };
    Tester t;
}

