/**
  *  \file test/afl/net/nullcommandhandlertest.cpp
  *  \brief Test for afl::net::NullCommandHandler
  */

#include "afl/net/nullcommandhandler.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.net.NullCommandHandler", a)
{
    afl::net::NullCommandHandler testee;

    // No args
    afl::data::Segment cmd;
    a.checkNull("call() returns null", testee.call(cmd));
    testee.callVoid(cmd);

    // Some args
    cmd.pushBackString("foo");
    cmd.pushBackInteger(10);
    a.checkNull("call(args) returns null", testee.call(cmd));
    testee.callVoid(cmd);
}
