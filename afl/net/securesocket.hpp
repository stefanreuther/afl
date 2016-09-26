/**
  *  \file afl/net/securesocket.hpp
  *  \brief Base Class afl::net::SecureSocket
  */
#ifndef AFL_AFL_NET_SECURESOCKET_HPP
#define AFL_AFL_NET_SECURESOCKET_HPP

#include "afl/net/socket.hpp"
#include "afl/base/ptr.hpp"
#include "afl/async/controller.hpp"

namespace afl { namespace net {

    /** Secure (SSL/TLS) Socket.
        This is the base class for sockets that perform encrypted data transfers.
        Like Socket, a SecureSocket is an asynchronous communication object. */
    class SecureSocket : public Socket {
     public:
        // FIXME: add some specific methods...
    };

} }

#endif
