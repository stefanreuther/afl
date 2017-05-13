/**
  *  \file afl/net/nullcommandhandler.hpp
  *  \brief Class afl::net::NullCommandHandler
  */
#ifndef AFL_AFL_NET_NULLCOMMANDHANDLER_HPP
#define AFL_AFL_NET_NULLCOMMANDHANDLER_HPP

#include "afl/net/commandhandler.hpp"

namespace afl { namespace net {

    /** Null CommandHandler.
        Ignores all commands and returns null. */
    class NullCommandHandler : public CommandHandler {
     public:
        virtual Value_t* call(const Segment_t& command);
        virtual void callVoid(const Segment_t& command);
    };

} }

#endif
