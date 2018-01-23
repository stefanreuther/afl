/**
  *  \file afl/test/networkstack.hpp
  *  \brief Class afl::test::NetworkStack
  */
#ifndef AFL_AFL_TEST_NETWORKSTACK_HPP
#define AFL_AFL_TEST_NETWORKSTACK_HPP

#include <list>
#include "afl/net/networkstack.hpp"
#include "afl/test/assert.hpp"
#include "afl/net/name.hpp"
#include "afl/base/memory.hpp"

namespace afl { namespace test {

    /** Mock: NetworkStack.
        Simulates a network stack.
        Implements connect() to produce a configured list of sockets (see afl::test::Socket).

        It is an error
        - to call connect() with the wrong / out-of-sequence Name
        - to call listen()
        - to do the wrong thing to the socket

        This is completely synchronous and not multi-thread safe. */
    class NetworkStack : public afl::net::NetworkStack {
     public:
        /** Constructor
            \param a Assertion generator */
        explicit NetworkStack(const Assert& a);

        // NetworkStack:
        virtual afl::base::Ref<afl::net::Listener> listen(const afl::net::Name& name, int backlogSize);
        virtual afl::base::Ref<afl::net::Socket> connect(const afl::net::Name& name, afl::sys::Timeout_t timeout);

        /** Expect a connection.
            \param name Name to report in the connection's getPeerName(), getName()
            \param in   Data to produce on receive(). Must have lifetime that exceeds that of the NetworkStack/Socket.
            \param out  Data to expect on send(). Must have lifetime that exceeds that of the NetworkStack/Socket. */
        void expectConnection(const afl::net::Name& name, afl::base::ConstBytes_t in, afl::base::ConstBytes_t out);

     private:
        const Assert m_assert;
        struct Connection {
            afl::net::Name name;
            afl::base::ConstBytes_t in;
            afl::base::ConstBytes_t out;
            Connection(const afl::net::Name& name, afl::base::ConstBytes_t in, afl::base::ConstBytes_t out)
                : name(name), in(in), out(out)
                { }
        };
        std::list<Connection> m_connections;
    };

} }

#endif
