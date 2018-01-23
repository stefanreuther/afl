/**
  *  \file afl/net/tunnel/socks4networkstack.hpp
  *  \brief Class afl::net::tunnel::Socks4NetworkStack
  */
#ifndef AFL_AFL_NET_TUNNEL_SOCKS4NETWORKSTACK_HPP
#define AFL_AFL_NET_TUNNEL_SOCKS4NETWORKSTACK_HPP

#include "afl/net/networkstack.hpp"
#include "afl/net/name.hpp"

namespace afl { namespace net { namespace tunnel {

    /** SOCKS4 tunnel.
        This implements the connect() and listen() operations using a SOCKS4/SOCKS4a server.
        The Name given to connect()/listen() is interpreted by the server.
        If it is a DNS name, the remote DNS is used.

        listen() has only limited support:
        - the server may not support it at all
        - the server may ignore our port number and listen on a different port, without informing us beforehand.
          This is probably intended to support active FTP, but our interface does not allow to use it.

        \todo Do we want to keep listen() despite these limitations? */
    class Socks4NetworkStack : public NetworkStack {
     public:
        Socks4NetworkStack(NetworkStack& parent, const Name& serverAddress);
        ~Socks4NetworkStack();

        virtual afl::base::Ref<Listener> listen(const Name& name, int backlogSize);
        virtual afl::base::Ref<Socket> connect(const Name& name, afl::sys::Timeout_t timeout = afl::sys::INFINITE_TIMEOUT);

     private:
        class ListenerImpl;
        class AcceptorImpl;

        NetworkStack& m_parent;
        Name m_serverAddress;
    };

} } }

#endif
