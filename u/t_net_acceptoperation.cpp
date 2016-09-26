/**
  *  \file u/t_net_acceptoperation.cpp
  *  \brief Test for afl::net::AcceptOperation
  */

#include "afl/net/acceptoperation.hpp"

#include "u/t_net.hpp"
#include "afl/net/socket.hpp"
#include "afl/net/name.hpp"

namespace {
    using afl::async::Controller;
    using afl::async::Operation;
    using afl::async::SendOperation;
    using afl::async::ReceiveOperation;

    // Helper. We just need a class derived from Socket which we can create.
    class NullSocket : public afl::net::Socket {
     public:
        // Socket
        virtual void closeSend() { }
        virtual afl::net::Name getPeerName() { return afl::net::Name("a","b"); }

        // CommunicationObject
        virtual bool send(Controller&, SendOperation&, afl::sys::Timeout_t) { return true; }
        virtual void sendAsync(Controller&, SendOperation&) { };
        virtual bool receive(Controller&, ReceiveOperation&, afl::sys::Timeout_t) { return true; }
        virtual void receiveAsync(Controller&, ReceiveOperation&) { }
        virtual String_t getName() { return "NullSocket"; }

        // Cancelable
        virtual void cancel(Controller&, Operation&) { }
    };
}

/** Simple test. */
void
TestNetAcceptOperation::testIt()
{
    // Default construction
    afl::net::AcceptOperation op;
    TS_ASSERT(op.getResult().get() == 0);

    // Derivation
    afl::async::Operation* base = &op;
    TS_ASSERT(base != 0);

    // Set/get result
    afl::base::Ptr<afl::net::Socket> sock(new NullSocket());
    TS_ASSERT(sock.get() != 0);
    op.setResult(sock);
    TS_ASSERT(op.getResult().get() != 0);
    TS_ASSERT(op.getResult() == sock);
}
