/**
  *  \file afl/net/resp/client.hpp
  *  \brief Class afl::net::resp::Client
  */
#ifndef AFL_AFL_NET_RESP_CLIENT_HPP
#define AFL_AFL_NET_RESP_CLIENT_HPP

#include "afl/async/controller.hpp"
#include "afl/base/memory.hpp"
#include "afl/base/ptr.hpp"
#include "afl/base/types.hpp"
#include "afl/data/defaultvaluefactory.hpp"
#include "afl/io/resp/parser.hpp"
#include "afl/net/commandhandler.hpp"
#include "afl/net/name.hpp"
#include "afl/net/networkstack.hpp"
#include "afl/net/reconnectable.hpp"
#include "afl/net/socket.hpp"
#include "afl/sys/mutex.hpp"

namespace afl { namespace net { namespace resp {

    /** Client for a RESP based protocol (redis).

        A Client can be used by multiple threads and serializes accesses accordingly.
        However, the application protocol may require external synchronisation for statefule multi-command sequences.

        resp::Client is a CommandHandler.
        Each command is sent to the server encoded as an array (multi-bulk).
        call() and callVoid() effectively have the same semantics.

        resp::Client is Reconnectable. */
    class Client : public CommandHandler,
                   public Reconnectable
    {
     public:
        /** Constructor.
            \param stack Network stack to use. Lifetime must be greater than the lifetime of the Client.
            \param name Network name to connect to */
        Client(NetworkStack& stack, const Name& name);

        /** Destructor. */
        ~Client();

        // CommandHandler:
        virtual Value_t* call(const Segment_t& command);
        virtual void callVoid(const Segment_t& command);

        // Reconnectable:
        virtual void setReconnectMode(Mode mode);

     private:
        // Red tape
        NetworkStack& m_stack;                    // Network stack
        const Name m_name;                        // Network name
        afl::sys::Mutex m_mutex;                  // Mutex for everything
        afl::data::DefaultValueFactory m_factory; // Value factory
        afl::async::Controller m_controller;      // Controller for socket access

        // Configuration
        Mode m_mode;

        // Network connection
        afl::base::Ptr<Socket> m_socket;          // Socket

        // Input
        uint8_t m_inputBuffer[4096];              // Input buffer data
        afl::base::ConstBytes_t m_input;          // Input buffer descriptor
        afl::io::resp::Parser m_parser;           // Input parser

        void sendCommand(const Segment_t& command);
        afl::data::Value* readResponse();
        void connect();
    };

} } }

#endif
