/**
  *  \file afl/net/tunnel/socks5networkstack.hpp
  *  \brief Class afl::net::tunnel::Socks5NetworkStack
  */
#ifndef AFL_AFL_NET_TUNNEL_SOCKS5NETWORKSTACK_HPP
#define AFL_AFL_NET_TUNNEL_SOCKS5NETWORKSTACK_HPP

#include "afl/net/networkstack.hpp"
#include "afl/net/name.hpp"

namespace afl { namespace net { namespace tunnel {

    /** SOCKS5 tunnel.
        This implements the connect() operation using a SOCKS5 server.
        The Name given to connect() is interpreted by the server.
        If it is a DNS name, the remote DNS is used.

        listen() is not supported and will throw an afl::except::UnsupportedException. */
    class Socks5NetworkStack : public NetworkStack {
     public:
        Socks5NetworkStack(NetworkStack& parent, const Name& serverAddress);
        ~Socks5NetworkStack();

        virtual afl::base::Ref<Listener> listen(const Name& name, int backlogSize);
        virtual afl::base::Ref<Socket> connect(const Name& name, afl::sys::Timeout_t timeout = afl::sys::INFINITE_TIMEOUT);

     private:
        NetworkStack& m_parent;
        Name m_serverAddress;
    };


} } }

#endif
