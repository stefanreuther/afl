/**
  *  \file afl/net/nullcommandhandler.cpp
  *  \brief Class afl::net::NullCommandHandler
  */

#include "afl/net/nullcommandhandler.hpp"

afl::net::CommandHandler::Value_t*
afl::net::NullCommandHandler::call(const Segment_t& /*command*/)
{
    return 0;
}

void
afl::net::NullCommandHandler::callVoid(const Segment_t& /*command*/)
{ }
