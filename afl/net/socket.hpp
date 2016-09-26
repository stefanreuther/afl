/**
  *  \file afl/net/socket.hpp
  *  \brief Base Class afl::net::Socket
  */
#ifndef AFL_AFL_NET_SOCKET_HPP
#define AFL_AFL_NET_SOCKET_HPP

#include "afl/sys/types.hpp"
#include "afl/async/communicationobject.hpp"

namespace afl { namespace net {

    class Name;

    /** Network socket.
        This is the base class for sockets that do actual data transfers.
        A socket is an asynchronous communication object.

        send/receive are allowed to do partial send/receive, you must check the return Operation.
        For full send/receives, use CommunicationStream::fullRead/fullWrite with the socket. */
    class Socket : public afl::async::CommunicationObject {
     public:
        /** Close sender end.
            This signals the receiving end of the socket connection an end-of-file condition. */
        virtual void closeSend() = 0;

        /** Get peer name for connected socket.
            \return Peer name */
        virtual Name getPeerName() = 0;
    };

} }

#endif
