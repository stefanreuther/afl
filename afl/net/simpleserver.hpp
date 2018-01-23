/**
  *  \file afl/net/simpleserver.hpp
  *  \brief Class afl::net::SimpleServer
  */
#ifndef AFL_AFL_NET_SIMPLESERVER_HPP
#define AFL_AFL_NET_SIMPLESERVER_HPP

#include "afl/base/ptr.hpp"
#include "afl/base/stoppable.hpp"
#include "afl/base/types.hpp"
#include "afl/base/ref.hpp"
#include "afl/sys/mutex.hpp"

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
    class SimpleServer : public afl::base::Stoppable {
     public:
        /** Constructor.
            \param listener Socket listener. Must not be null.
            \param factory ProtocolHandler factory. Must have sufficient (=infinite) lifetime. */
        SimpleServer(afl::base::Ref<Listener> listener, ProtocolHandlerFactory& factory);

        /** Destructor. */
        ~SimpleServer();

        /** Run this SimpleServer. This will accept and handle connections. */
        virtual void run();

        /** Request stop.
            The SimpleServer will stop after processing the current/next request.

            <b>Caveat:</b> This will NOT immediately stop the SimpleServer.
            You need to trigger a connection attempt to make it register the request.
            When called from a running ProtocolHandler, this will terminate the SimpleServer after the connection completes. */
        virtual void stop();

     private:
        /** Listener. */
        afl::base::Ref<Listener> m_listener;

        /** ProtocolHandler factory. */
        ProtocolHandlerFactory& m_factory;

        /** Stop request mutex. */
        afl::sys::Mutex m_mutex;

        /** Stop request flag. */
        bool m_stopRequested;

        bool isStopRequested();
    };

} }

#endif
