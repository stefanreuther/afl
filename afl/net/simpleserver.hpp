/**
  *  \file afl/net/simpleserver.hpp
  *  \brief Class afl::net::SimpleServer
  */
#ifndef AFL_AFL_NET_SIMPLESERVER_HPP
#define AFL_AFL_NET_SIMPLESERVER_HPP

#include "afl/base/ptr.hpp"
#include "afl/base/runnable.hpp"
#include "afl/base/types.hpp"

namespace afl { namespace net {

    class Listener;
    class Socket;
    class ProtocolHandlerFactory;

    /** Simple single-connection server.
        This implements a simple server for a ProtocolHandler.

        Limitations:
        - single connection only (but multiple SimpleServer threads can share a Listener,
          or even a ProtocolHandlerFactory if that one supports it)
        - cannot be terminated, i.e. runs forever */
    class SimpleServer : public afl::base::Runnable {
     public:
        /** Constructor.
            \param listener Socket listener. Must not be null.
            \param factory ProtocolHandler factory. Must have sufficient (=infinite) lifetime. */
        SimpleServer(afl::base::Ptr<Listener> listener, ProtocolHandlerFactory& factory);

        /** Destructor. */
        ~SimpleServer();

        /** Run this SimpleServer. This will accept and handle connections. */
        virtual void run();

     private:
        /** Listener. */
        afl::base::Ptr<Listener> m_listener;

        /** ProtocolHandler factory. */
        ProtocolHandlerFactory& m_factory;

        /** Buffer for receiving.
            Allocated within the object on the heap, so we don't have to allocate it separately,
            on the stack or elsewhere. */
        uint8_t m_buffer[4096];

        /** Handle a connection.
            Implements the whole connection lifetime.
            \param socket Socket to work on */
        void handleConnection(Socket& socket);
    };

} }

#endif
