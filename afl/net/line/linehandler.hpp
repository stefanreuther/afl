/**
  *  \file afl/net/line/linehandler.hpp
  *  \brief Interface afl::net::line::LineHandler
  */
#ifndef AFL_AFL_NET_LINE_LINEHANDLER_HPP
#define AFL_AFL_NET_LINE_LINEHANDLER_HPP

#include "afl/base/deletable.hpp"
#include "afl/string/string.hpp"

namespace afl { namespace net { namespace line {

    class LineSink;

    /** Line-based protocol handler.
        A LineHandler descendant implements the client or server side of a line-based interaction protocol.
        Functions are called in this sequence:
        - handleOpening() to produce a possible opening. For a server side, this could be a greeting;
          for a client side, this would send a command.
        - handleLine() is called repeatedly for lines received from the other side.
        - handleConnectionClose() is called when the other side closed the connection.
          It is not called when this side initiated the connection closing.

        handleOpening() and handleLine() can request a conversation to stop.
        For a client, this stops processing.
        For a server, this closes the connection.
        These methods can also cause text to be sent to the other side, by using LineSink::handleLine(). */
    class LineHandler : public afl::base::Deletable {
     public:
        /** Handle opening of a conversation.
            Called when the conversation starts.
            For a server, this is when a new client connects.
            For a client, this is the start of a request, and this method could send a command.
            \param response LineSink to send text on
            \return true to end conversation, false to continue */
        virtual bool handleOpening(LineSink& response) = 0;

        /** Handle a received line.
            Called when a line has been received from the other side.
            \param line Received line
            \param response LineSink to send text on
            \return true to end conversation, false to continue */
        virtual bool handleLine(const String_t& line, LineSink& response) = 0;

        /** Handle closing of a connection.
            Called when the other side has closed the connection. */
        virtual void handleConnectionClose() = 0;
    };

} } }

#endif
