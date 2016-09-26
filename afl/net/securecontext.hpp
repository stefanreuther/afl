/**
  *  \file afl/net/securecontext.hpp
  *  \brief class afl::net::SecureContext
  */
#ifndef AFL_AFL_NET_SECURECONTEXT_HPP
#define AFL_AFL_NET_SECURECONTEXT_HPP

#include "afl/base/refcounted.hpp"
#include "afl/async/controller.hpp"
#include "afl/base/ptr.hpp"
#include "afl/net/socket.hpp"
#include "afl/base/memory.hpp"

namespace afl { namespace net {

    class SecureSocket;

    /** Secure (SSL/TLS) Socket Context.
        This class manages context required to establish a secure socket connection.

        Lifetime: a SecureContext must always be allocated on the heap and managed by afl::base::Ptr;
        this makes sure that if multiple sockets need it, it remains alive long enough.

        Multithreading: SecureContext must be properly interlocked
        so users can use one context from multiple threads. */
    class SecureContext : public afl::base::RefCounted {
     public:
        /** Default constructor. */
        SecureContext();

        /** Virtual destructor. */
        virtual ~SecureContext();

        /** Wrap client socket into SecureSocket.
            This will perform the SSL/TLS handshake from the client viewpoint.
            \param ctl Controller to perform I/O
            \param peer existing socket
            \return newly-allocated SecureSocket */
        virtual afl::base::Ptr<SecureSocket> wrapClient(afl::async::Controller& ctl, afl::base::Ptr<Socket> peer) = 0;

        /** Wrap server socket into SecureSocket.
            This will perform the SSL/TLS handshake from the server viewpoint.
            You must previously have called setPrivateKey(), setCertificate().
            \param ctl Controller to perform I/O
            \param peer existing socket
            \return newly-allocated SecureSocket */
        virtual afl::base::Ptr<SecureSocket> wrapServer(afl::async::Controller& ctl, afl::base::Ptr<Socket> peer) = 0;

        virtual void setPrivateKey(afl::base::ConstBytes_t data) = 0;

        virtual void setCertificate(afl::base::ConstBytes_t data) = 0;

        /** Create a SecureContext.
            This is the configuration-dependant implementation that will create the
            configuration-dependant concreate instance of SecureContext.
            \throw afl::except::UnsupportedException if no SSL library is configured
            \return newly-allocated SecureContext */
        static afl::base::Ptr<SecureContext> create();
    };

} }

inline
afl::net::SecureContext::SecureContext()
{ }

inline
afl::net::SecureContext::~SecureContext()
{ }

#endif
