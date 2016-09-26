/**
  *  \file afl/net/protocolhandlerfactory.hpp
  *  \brief Interface afl::net::ProtocolHandlerFactory
  */
#ifndef AFL_AFL_NET_PROTOCOLHANDLERFACTORY_HPP
#define AFL_AFL_NET_PROTOCOLHANDLERFACTORY_HPP

#include "afl/base/deletable.hpp"

namespace afl { namespace net {

    class ProtocolHandler;

    /** Factory for ProtocolHandler. */
    class ProtocolHandlerFactory : public afl::base::Deletable {
     public:
        /** Create a ProtocolHandler.
            \return newly-allocated ProtocolHandler. */
        virtual ProtocolHandler* create() = 0;
    };

} }

#endif
