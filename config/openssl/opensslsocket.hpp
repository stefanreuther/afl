/**
  *  \file config/openssl/opensslsocket.hpp
  *  \brief Class config::openssl::OpenSSLSocket
  */
#ifndef AFL_CONFIG_OPENSSL_OPENSSLSOCKET_HPP
#define AFL_CONFIG_OPENSSL_OPENSSLSOCKET_HPP

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "afl/net/securesocket.hpp"
#include "afl/async/notifier.hpp"
#include "afl/async/controller.hpp"
#include "afl/base/ref.hpp"
#include "afl/async/sendoperation.hpp"
#include "afl/async/receiveoperation.hpp"
#include "config/openssl/opensslcontext.hpp"

namespace config { namespace openssl {

    /** SecureSocket implementation using OpenSSL.
        Implements a bidirectional, asynchronous, encrypted connection.
        Use afl::net::SecureSocket static methods to construct objects of this type.

        Limitations: timeouts/cancel will mess up the connection and cause all following operations to fail.
        This is because a cancel has to cancel the underlying transmission
        and we don't know how far it proceeded so far. */
    class OpenSSLSocket : public afl::net::SecureSocket, private afl::async::Notifier {
     public:
        /** Constructor.
            \param peer Underlying socket */
        OpenSSLSocket(afl::base::Ref<OpenSSLContext> ctx, afl::base::Ref<Socket> peer);

        /** Virtual destructor. */
        ~OpenSSLSocket();

        // Socket:
        virtual void closeSend();
        virtual afl::net::Name getPeerName();

        // CommunicationObject:
        virtual bool send(afl::async::Controller& ctl, afl::async::SendOperation& op, afl::sys::Timeout_t timeout = afl::sys::INFINITE_TIMEOUT);
        virtual void sendAsync(afl::async::Controller& ctl, afl::async::SendOperation& op);
        virtual bool receive(afl::async::Controller& ctl, afl::async::ReceiveOperation& op, afl::sys::Timeout_t timeout = afl::sys::INFINITE_TIMEOUT);
        virtual void receiveAsync(afl::async::Controller& ctl, afl::async::ReceiveOperation& op);
        virtual void cancel(afl::async::Controller& ctl, afl::async::Operation& op);
        virtual String_t getName();

        // OpenSSLSocket:
        void connect(afl::async::Controller& ctl);
        void accept(afl::async::Controller& ctl);

     private:
        // Existing socket
        afl::base::Ref<Socket> m_peer;

        // Mutex for following stuff
        afl::sys::Mutex m_mutex;

        // SSL crap
        SSL* m_ssl;
        BIO* m_sslEnd;
        BIO* m_myEnd;

        // Pending user actions
        struct Action {
            enum Kind {
                Connect,
                Accept,
                Send,
                Receive,
                Flush
            };
            Kind m_kind;
            afl::async::Operation* m_pOperation;
            Action(Kind kind, afl::async::Operation* pOperation)
                : m_kind(kind), m_pOperation(pOperation)
                { }
        };
        std::list<Action> m_actions;

        // Internal state
        enum State {
            Idle,               // Not doing anything
            Sending,            // OpenSSL told us to send because we want to perform some user operation
            Receiving,          // OpenSSL told us to receive because we want to perform some user operation
            Failed,             // Connection failed (no more reading/writing, error case)
            Closed              // Connection closed (no more reading, ok case)
        };
        State m_state;
        afl::async::SendOperation m_sendOperation;
        afl::async::ReceiveOperation m_receiveOperation;

        bool m_connectStatus;
        unsigned long m_connectError;

        uint8_t m_buffer[1024];

        // Notifier
        virtual void notify(afl::async::Operation& op);

        enum ResultClass {
            Failure,
            Success,
            Wait
        };

        /** Classify SSL syscall result.
            Performs initial processing and returns the result class of type Result.
            \param result SSL syscall result
            \return result class */
        ResultClass classifyResult(int result);

        void cycle();
        bool cycleConnect(Action& a);
        bool cycleAccept(Action& a);
        bool cycleSend(Action& a);
        bool cycleReceive(Action& a);
        bool cycleFlush();
    };

} }

#endif
