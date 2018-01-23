/**
  *  \file afl/net/line/protocolhandler.hpp
  *  \brief Class afl::net::line::ProtocolHandler
  */
#ifndef AFL_AFL_NET_LINE_PROTOCOLHANDLER_HPP
#define AFL_AFL_NET_LINE_PROTOCOLHANDLER_HPP

#include <memory>
#include "afl/net/protocolhandler.hpp"
#include "afl/base/growablememory.hpp"
#include "afl/net/line/linesink.hpp"

namespace afl { namespace net { namespace line {

    class LineHandler;

    /** Protocol handler for a line-based interaction protocol.

        For a stateless protocol, have your ProtocolHandlerFactory create a ProtocolHandler,
        refering to a single LineHandler that interprets the input.

        For a stateful (=per-session state) protocol, derive your own ProtocolHandler class.
        Create an instance of your LineHandler and one of afl::net::line::ProtocolHandler,
        and dispatch calls into your ProtocolHandler to the afl::net::line::ProtocolHandler instance. */
    class ProtocolHandler : public afl::net::ProtocolHandler,
                            private afl::net::line::LineSink
    {
     public:
        /** Constructor.
            \param lineHandler Protocol implementation */
        explicit ProtocolHandler(LineHandler& lineHandler);

        /** Destructor. */
        ~ProtocolHandler();

        // ProtocolHandler operations:
        virtual void getOperation(Operation& op);
        virtual void advanceTime(afl::sys::Timeout_t msecs);
        virtual void handleData(afl::base::ConstBytes_t bytes);
        virtual void handleSendTimeout(afl::base::ConstBytes_t unsentBytes);
        virtual void handleConnectionClose();

     private:
        LineHandler& m_lineHandler;

        String_t m_input;

        bool m_opened;
        bool m_closeRequested;

        std::auto_ptr<afl::base::GrowableBytes_t> m_currentOutput;
        std::auto_ptr<afl::base::GrowableBytes_t> m_futureOutput;

        // LineSink:
        virtual void handleLine(const String_t& line);
    };

} } }

#endif
