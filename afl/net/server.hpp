/**
  *  \file afl/net/server.hpp
  *  \brief Class afl::net::Server
  */
#ifndef AFL_AFL_NET_SERVER_HPP
#define AFL_AFL_NET_SERVER_HPP

#include "afl/async/controller.hpp"
#include "afl/async/semaphore.hpp"
#include "afl/base/ref.hpp"
#include "afl/base/stoppable.hpp"
#include "afl/container/ptrvector.hpp"
#include "afl/sys/log.hpp"

namespace afl { namespace net {

    class Listener;
    class ProtocolHandlerFactory;

    /** Network server.
        This implements a server for a ProtocolHandler.
        It accepts multiple connections from a Listener
        and serves them using the ProtocolHandler's created by the ProtocolHandlerFactory.
        All processing happens in a single thread (i.e. this class' run() method).

        Server has an internal Log node which you can subscribe to receive log messages.

        Typical setup (e.g. in your wrapper class' constructor):
        - create ProtocolHandlerFactory descendant
        - create a Server and configure it (i.e. setLogName)
        - create a Thread and call Thread::start

        Typical shutdown (e.g. in your wrapper class' destructor):
        - call Server::stop
        - call Thread::join
        - destroy everything */
    class Server : public afl::base::Stoppable {
     public:
        /** Constructor.
            \param listener Listener
            \param factory ProtocolHandlerFactory. Must live longer than the Server. */
        Server(afl::base::Ref<Listener> listener, ProtocolHandlerFactory& factory);

        /** Destructor. */
        ~Server();

        /** Entry point.
            Processes connections until stop() is called. */
        virtual void run();

        /** Stop this server.
            Can be called from any thread.
            Will cause run() to exit. */
        virtual void stop();

        /** Set name of log channel.
            Must be called before run() starts. */
        void setLogName(const String_t& logName);

        /** Get log node.
            \return log node */
        afl::sys::Log& log();

     private:
        struct StopState;
        struct ListenerState;
        struct ConnectionState;

        /** Listener. */
        afl::base::Ref<Listener> m_listener;

        /** ProtocolHandler factory. */
        ProtocolHandlerFactory& m_factory;

        /** Semaphore to signal stop. */
        afl::async::Semaphore m_stopSignal;

        /** Log node. */
        afl::sys::Log m_log;

        /** Log channel name. */
        String_t m_logName;

        /** Close signal.
            Set to true if one connection wants to close. */
        bool m_closeSignal;

        void startStop(afl::async::Controller& ctl, StopState& state);
        void startListen(afl::async::Controller& ctl, ListenerState& state);
        void startConnection(afl::async::Controller& ctl, ConnectionState& state);

        void stopListen(afl::async::Controller& ctl, ListenerState& state);
        void stopConnection(afl::async::Controller& ctl, ConnectionState& state);

        bool handleConnectionEvent(afl::async::Controller& ctl, ConnectionState& state, afl::async::Operation* op);
        void handleConnectionTime(afl::async::Controller& ctl, ConnectionState& state, uint32_t now);

        void logException(const String_t& name, const char* op, const char* what);

        static uint32_t findTimeout(const afl::container::PtrVector<ConnectionState>& connections, uint32_t now);
    };

} }

#endif
