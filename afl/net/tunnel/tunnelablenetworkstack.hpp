/**
  *  \file afl/net/tunnel/tunnelablenetworkstack.hpp
  *  \brief Class afl::net::tunnel::TunnelableNetworkStack
  */
#ifndef AFL_AFL_NET_TUNNEL_TUNNELABLENETWORKSTACK_HPP
#define AFL_AFL_NET_TUNNEL_TUNNELABLENETWORKSTACK_HPP

#include "afl/net/networkstack.hpp"
#include "afl/net/url.hpp"
#include "afl/base/deleter.hpp"

namespace afl { namespace net { namespace tunnel {

    /** Network stack for optional tunneling.
        Implements the interface of NetworkStack and allows runtime addition of tunnels.
        To enable an application to use tunneling, use a TunnelableNetworkStack instance
        in place of the system's afl::net::NetworkStack instance, and give the user a way to configure it (by calling add()).

        TunnelableNetworkStack maintains the lifetime of the tunnel implementation objects.
        It supports multi-hop tunneling: to tunnel from the current system to A, from there to B, then to C which
        accesses the public internet, call add(A), add(B), add(C) in that order.

        A tunnel is specified by an URL string.
        We support the formats:
        - socks4://host:port (Socks4NetworkStack)
        - socks5://host:port (Socks5NetworkStack)
        See these classes for restrictions.

        The TunnelableNetworkStack is created with no tunnel configured, accessing the underlying network directly. */
    class TunnelableNetworkStack : public NetworkStack {
     public:
        /** Constructor.
            \param base Underlying network stack */
        explicit TunnelableNetworkStack(NetworkStack& base);

        /** Destructor. */
        ~TunnelableNetworkStack();

        /** Add tunnel (string).
            \param link Tunnel link in text form (see class description)
            \return true if link could be processed correctly (no statement whether the tunnel server can be reached) */
        bool add(const String_t& link);

        /** Add tunnel (URL).
            \param link Tunnel link in parsed Url form (see class description)
            \return true if link could be processed correctly (no statement whether the tunnel server can be reached) */
        bool add(const Url& url);

        /** Reset.
            Discards all tunnel configuration.
            Future accesses will hit the underlying network directly. */
        void reset();

        // NetworkStack:
        virtual afl::base::Ref<Listener> listen(const Name& name, int backlogSize);
        virtual afl::base::Ref<Socket> connect(const Name& name, afl::sys::Timeout_t timeout = afl::sys::INFINITE_TIMEOUT);

     private:
        NetworkStack& m_base;
        NetworkStack* m_current;
        afl::base::Deleter m_deleter;
    };

} } }

#endif
