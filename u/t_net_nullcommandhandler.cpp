/**
  *  \file u/t_net_nullcommandhandler.cpp
  *  \brief Test for afl::net::NullCommandHandler
  */

#include "afl/net/nullcommandhandler.hpp"

#include "t_net.hpp"

/** Simple test. */
void
TestNetNullCommandHandler::testIt()
{
    afl::net::NullCommandHandler testee;

    // No args
    afl::data::Segment cmd;
    TS_ASSERT(testee.call(cmd) == 0);
    testee.callVoid(cmd);

    // Some args
    cmd.pushBackString("foo");
    cmd.pushBackInteger(10);
    TS_ASSERT(testee.call(cmd) == 0);
    testee.callVoid(cmd);
}
