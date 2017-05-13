/**
  *  \file afl/net/resp/protocolhandler.hpp
  *  \brief Class afl::net::resp::ProtocolHandler
  */
#ifndef AFL_AFL_NET_RESP_PROTOCOLHANDLER_HPP
#define AFL_AFL_NET_RESP_PROTOCOLHANDLER_HPP

#include "afl/container/ptrqueue.hpp"
#include "afl/data/defaultvaluefactory.hpp"
#include "afl/io/internalsink.hpp"
#include "afl/io/resp/parser.hpp"
#include "afl/net/commandhandler.hpp"
#include "afl/net/protocolhandler.hpp"
#include "afl/data/value.hpp"

namespace afl { namespace net { namespace resp {

    /** Protocol handler for a RESP command protocol.

        For a stateless protocol, have your ProtocolHandlerFactory create a ProtocolHandler,
        refering to a single CommandHandler that interprets the commands.

        For a stateful (=per-session state) protocol, derive your own ProtocolHandler class.
        Create an instance of your CommandHandler and one of afl::net::resp::ProtocolHandler,
        and dispatch calls into your ProtocolHandler to the afl::net::resp::ProtocolHandler instance. */
    class ProtocolHandler : public afl::net::ProtocolHandler {
     public:
        /** Constructor.
            \param ch Command interpreter */
        explicit ProtocolHandler(CommandHandler& ch);

        /** Destructor. */
        ~ProtocolHandler();

        // ProtocolHandler operations:
        virtual void getOperation(Operation& op);
        virtual void advanceTime(afl::sys::Timeout_t msecs);
        virtual void handleData(afl::base::ConstBytes_t bytes);
        virtual void handleSendTimeout(afl::base::ConstBytes_t unsentBytes);
        virtual void handleConnectionClose();

     private:
        enum State {
            Idle,
            Sending,
            Failed
        };
        State m_state;
        CommandHandler& m_ch;
        afl::container::PtrQueue<afl::io::InternalSink> m_data;
        afl::data::DefaultValueFactory m_factory;
        afl::io::resp::Parser m_parser;

        void handleNewValue(afl::data::Value* p);
    };

} } }

#endif
