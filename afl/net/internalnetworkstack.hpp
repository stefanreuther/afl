/**
  *  \file afl/net/internalnetworkstack.hpp
  *  \brief Class afl::net::InternalNetworkStack
  */
#ifndef AFL_AFL_NET_INTERNALNETWORKSTACK_HPP
#define AFL_AFL_NET_INTERNALNETWORKSTACK_HPP

#include <map>
#include "afl/base/refcounted.hpp"
#include "afl/net/listener.hpp"
#include "afl/net/name.hpp"
#include "afl/net/networkstack.hpp"
#include "afl/net/socket.hpp"
#include "afl/sys/mutex.hpp"

namespace afl { namespace net {

    /** Internal implementation of NetworkStack for testing.
        Listener and Socket interfaces are implemented entirely internal,
        with no effect on the system's real network stack.
        Therefore, operations on an InternalNetworkStack will never clash with operations on a real NetworkStack,
        or other InternalNetworkStack instances.

        Network names can be arbitrary strings.
        A connect() for a name is paired with a listen() for the same name.
        Only one listen() for a name can be active at a time (like system-wide on real NetworkStack).
        A listener must be available before a connect operation can succeed,
        there is no "grace period" for connect-before-listen as in a real network.

        InternalNetworkStack does not implement the listen() backlogSize parameter.
        Any number of connection attempts can be made.

        InternalNetworkStack does not implement any buffering.
        A send() completes at the same moment the receive() taking its data completes.

        A socket created from listen()/accept() reports the name passed to listen as its getName().
        Consequently, a socket created from connect() reports that name as its getPeerName().
        We make no guarantees about the other direction's name (but for now, it's the same).

        InternalNetworkStack must be created on the heap using InternalNetworkStack::create().
        It lives as long as a Listener or Socket it created is alive. */
    class InternalNetworkStack : public NetworkStack, public afl::base::RefCounted {
     public:
        /** A pair of sockets. */
        typedef std::pair<afl::base::Ref<Socket>, afl::base::Ref<Socket> > SocketPair_t;

        /** Create an InternalNetworkStack.
            @return newly-created instance */
        static afl::base::Ref<InternalNetworkStack> create();

        /** Create a pair of connected sockets.
            @param name Name that is reported as getName(), getPeerName()
            @return two InternalNetworkStack sockets that can be used to send data between each other */
        static SocketPair_t createSocketPair(const Name& name);

        // NetworkStack implementation
        virtual afl::base::Ref<Listener> listen(const Name& name, int backlogSize);
        virtual afl::base::Ref<Socket> connect(const Name& name, afl::sys::Timeout_t timeout = afl::sys::INFINITE_TIMEOUT);

     private:
        InternalNetworkStack();

        class InternalListener;
        class InternalSocket;
        class Stream;

        afl::sys::Mutex m_mutex;
        std::map<String_t, InternalListener*> m_listeners;

        void addListener(const Name& name, InternalListener& lis);
        void removeListener(const Name& name);
    };

} }

#endif
