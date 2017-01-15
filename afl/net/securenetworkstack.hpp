/**
  *  \file afl/net/securenetworkstack.hpp
  *  \brief Class afl::net::SecureNetworkStack
  */
#ifndef AFL_AFL_NET_SECURENETWORKSTACK_HPP
#define AFL_AFL_NET_SECURENETWORKSTACK_HPP

#include "afl/net/networkstack.hpp"
#include "afl/net/securecontext.hpp"

namespace afl { namespace net {

    /** Secure Network Stack.
        Implements the NetworkStack interface using SecureSocket's.
        That is, all data transfer going through a SecureNetworkStack will be encrypted. */
    class SecureNetworkStack : public NetworkStack {
     public:
        /** Constructor.
            Makes a SecureNetworkStack that operates on the underlying real network stack.
            \param peer Underlying network stack
            \param ctx SecureContext to use. If not specified, uses an unconfigured default instance
                       which is good for usage as a client. */
        explicit SecureNetworkStack(NetworkStack& peer, afl::base::Ref<SecureContext> ctx = SecureContext::create());

        virtual ~SecureNetworkStack();

        // NetworkStack methods:
        virtual afl::base::Ref<afl::net::Listener> listen(const Name& name, int backlogSize);
        virtual afl::base::Ref<afl::net::Socket> connect(const Name& name, afl::sys::Timeout_t timeout = afl::sys::INFINITE_TIMEOUT);

     private:
        NetworkStack& m_peer;
        afl::base::Ref<SecureContext> m_ctx;

        class Listener;
    };

} }

#endif
