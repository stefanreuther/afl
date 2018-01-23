/**
  *  \file afl/net/line/client.hpp
  *  \brief Class afl::net::line::Client
  */
#ifndef AFL_AFL_NET_LINE_CLIENT_HPP
#define AFL_AFL_NET_LINE_CLIENT_HPP

#include "afl/net/networkstack.hpp"
#include "afl/net/name.hpp"
#include "afl/sys/mutex.hpp"
#include "afl/async/controller.hpp"

namespace afl { namespace net { namespace line {

    class LineHandler;

    /** Client for a line-based interaction protocol.

        A Client can be used by multiple threads and serializes accesses accordingly.
        However, the application protocol may require external synchronisation for stateful multi-command sequences.

        Each command to send is implemented as a LineHandler.
        This LineHandler defines how to send a request (handleOpening()),
        and how to receive a response (handleLine()).
        By returning true, these functions signal the end of the interaction. */
    class Client {
     public:
        /** Constructor.
            \param stack Network stack to use. Lifetime must be greater than the lifetime of the Client.
            \param name Network name to connect to */
        Client(NetworkStack& stack, const Name& name);

        /** Destructor. */
        ~Client();

        /** Perform a request.
            The LineHandler describes what to send and what to receive.
            A possible result, if any, can be queried from the LineHandler. */
        void call(LineHandler& request);

     private:
        // Red tape
        NetworkStack& m_stack;                    // Network stack
        const Name m_name;                        // Network name
        afl::sys::Mutex m_mutex;                  // Mutex for everything
        afl::async::Controller m_controller;      // Controller for socket access

        // Network connection
        afl::base::Ptr<Socket> m_socket;          // Socket

        uint8_t m_bytes[4096];
        afl::base::ConstBytes_t m_buffer;

        void connect();
    };

} } }

#endif
