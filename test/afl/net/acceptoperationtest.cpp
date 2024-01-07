/**
  *  \file test/afl/net/acceptoperationtest.cpp
  *  \brief Test for afl::net::AcceptOperation
  */

#include "afl/net/acceptoperation.hpp"

#include "afl/net/name.hpp"
#include "afl/net/socket.hpp"
#include "afl/test/testrunner.hpp"

namespace {
    using afl::async::Controller;
    using afl::async::Operation;
    using afl::async::ReceiveOperation;
    using afl::async::SendOperation;

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
AFL_TEST("afl.net.AcceptOperation", a)
{
    // Default construction
    afl::net::AcceptOperation op;
    a.checkNull("01. getResult", op.getResult().get());

    // Derivation
    afl::async::Operation* base = &op;
    a.checkNonNull("02. base", base);

    // Set/get result
    afl::base::Ptr<afl::net::Socket> sock(new NullSocket());
    a.checkNonNull("03. socket", sock.get());
    op.setResult(sock);
    a.checkNonNull("04. getResult", op.getResult().get());
    a.check("05. getResult", op.getResult() == sock);
}
