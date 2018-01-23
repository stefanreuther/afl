/**
  *  \file afl/net/interact.hpp
  *  \brief Function afl::net::interact()
  */
#ifndef AFL_AFL_NET_INTERACT_HPP
#define AFL_AFL_NET_INTERACT_HPP

#include "afl/async/communicationobject.hpp"

namespace afl { namespace net {

    class Socket;
    class ProtocolHandler;

    /** Operate a ProtocolHandler on a given CommunicationObject.
        This drives a ProtocolHandler synchronously (using send() and receive()).
        Usecases include simple servers (one thread per request),
        operating a network or other connection.

        The function exits when the connection is closed
        (by the communication object reporting close, or the ProtocolHandler requesting it).
        The function also propagates exceptions thrown by the ProtocolHandler.

        \param obj     Communication object (socket, pipe, etc.)
        \param handler ProtocolHandler describing the protocol */
    void interact(afl::async::CommunicationObject& obj, ProtocolHandler& handler);

} }

#endif
