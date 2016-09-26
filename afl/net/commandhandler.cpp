/**
  *  \file afl/net/commandhandler.cpp
  *  \brief Interface afl::net::CommandHandler
  */

#include <memory>
#include "afl/net/commandhandler.hpp"
#include "afl/data/access.hpp"

int32_t
afl::net::CommandHandler::callInt(const afl::data::Segment& command)
{
    std::auto_ptr<Value_t> result(call(command));
    return afl::data::Access(result.get()).toInteger();
}

String_t
afl::net::CommandHandler::callString(const afl::data::Segment& command)
{
    std::auto_ptr<Value_t> result(call(command));
    return afl::data::Access(result.get()).toString();
}
