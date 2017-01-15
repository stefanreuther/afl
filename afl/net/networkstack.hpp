/**
  *  \file afl/net/networkstack.hpp
  *  \brief Interface afl::net::NetworkStack
  */
#ifndef AFL_AFL_NET_NETWORKSTACK_HPP
#define AFL_AFL_NET_NETWORKSTACK_HPP

#include "afl/base/ref.hpp"
#include "afl/net/listener.hpp"
#include "afl/net/socket.hpp"
#include "afl/sys/types.hpp"

namespace afl { namespace net {

    class Name;

    /** Network stack.
        An object of this type implements a network stack,
        and operates as a factory for network-related objects.
        There is a platform-specific implementation. */
    class NetworkStack : public afl::base::Deletable {
     public:
        /** Listen on a socket.
            This wraps the socket(), bind(), and listen() system calls around some name resolution.
            \param name Listen to this interface and port
            \param backlogSize Maximum number of connections to accept */
        virtual afl::base::Ref<Listener> listen(const Name& name, int backlogSize) = 0;

        /** Connect to a socket.
            This wraps the socket() and connect() system calls around some name resolution.
            \param name Connect to this interface and port
            \param timeout Maximum time to spend connecting
            \throw FileProblemException on error or timeout */
        virtual afl::base::Ref<Socket> connect(const Name& name, afl::sys::Timeout_t timeout = afl::sys::INFINITE_TIMEOUT) = 0;

        /** Get system network stack. */
        static NetworkStack& getInstance();
    };

} }

#endif
