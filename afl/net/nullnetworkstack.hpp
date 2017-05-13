/**
  *  \file afl/net/nullnetworkstack.hpp
  *  \brief Class afl::net::NullNetworkStack
  */
#ifndef AFL_AFL_NET_NULLNETWORKSTACK_HPP
#define AFL_AFL_NET_NULLNETWORKSTACK_HPP

#include "afl/net/networkstack.hpp"

namespace afl { namespace net {

    /** Null NetworkStack.
        This NetworkStack implementation immediately fails every request.
        It can be used if an interface requires a NetworkStack instance but does not actually use it,
        e.g. for testing. */
    class NullNetworkStack : public NetworkStack {
     public:
        NullNetworkStack();
        ~NullNetworkStack();

        // NetworkStack:
        virtual afl::base::Ref<Listener> listen(const Name& name, int backlogSize);
        virtual afl::base::Ref<Socket> connect(const Name& name, afl::sys::Timeout_t timeout = afl::sys::INFINITE_TIMEOUT);
    };

} }

#endif
