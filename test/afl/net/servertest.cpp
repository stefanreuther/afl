/**
  *  \file test/afl/net/servertest.cpp
  *  \brief Test for afl::net::Server
  */

#include "afl/net/server.hpp"

#include <memory>
#include "afl/async/receiveoperation.hpp"
#include "afl/async/sendoperation.hpp"
#include "afl/net/name.hpp"
#include "afl/net/networkstack.hpp"
#include "afl/net/protocolhandler.hpp"
#include "afl/net/protocolhandlerfactory.hpp"
#include "afl/sys/thread.hpp"
#include "afl/test/testrunner.hpp"

namespace {
    // A protocol handler for testing.
    // - sending a packet containing "q" closes the connection
    // - everything else is echoed back
    class Handler : public afl::net::ProtocolHandler {
     public:
        Handler(afl::test::Assert a)
            : m_assert(a), m_sent(true)
            { }

        virtual void getOperation(Operation& op)
            {
                if (!m_sent && m_buffer.size() > 0) {
                    if (m_buffer[0] == 'q') {
                        op.m_close = true;
                    } else {
                        op.m_dataToSend = afl::base::Bytes_t(m_buffer);
                    }
                }
                m_sent = true;
            }

        virtual void advanceTime(afl::sys::Timeout_t /*msecs*/)
            { }

        virtual void handleData(afl::base::ConstBytes_t bytes)
            {
                m_assert.check("handleData: m_sent", m_sent);
                m_assert.check("handleData: !bytes.empty", !bytes.empty());
                m_buffer.assign(bytes.unsafeData(), bytes.unsafeData() + bytes.size());
                m_sent = false;
            }

        virtual void handleSendTimeout(afl::base::ConstBytes_t /*unsentBytes*/)
            { }

        virtual void handleConnectionClose()
            { }

     private:
        afl::test::Assert m_assert;
        std::vector<uint8_t> m_buffer;
        bool m_sent;
    };

    class Factory : public afl::net::ProtocolHandlerFactory {
     public:
        Factory(afl::test::Assert a)
            : m_assert(a)
            { }
        Handler* create()
            {
                return new Handler(m_assert);
            }
     private:
        afl::test::Assert m_assert;
    };

    class ServerThread {
     public:
        ServerThread(afl::test::Assert a, afl::base::Ref<afl::net::Listener> listener)
            : m_factory(a),
              m_server(listener, m_factory),
              m_thread("Server", m_server)
            {
                m_thread.start();
            }
        ~ServerThread()
            {
                m_server.stop();
                m_thread.join();
            }
     private:
        Factory m_factory;
        afl::net::Server m_server;
        afl::sys::Thread m_thread;
    };
}

/** Simple test. */
AFL_TEST("afl.net.Server:connect", a)
{
    // Network stack
    afl::net::NetworkStack& stack = afl::net::NetworkStack::getInstance();
    afl::net::Name name("localhost", uint16_t(std::rand() % 30000 + 20000));

    // Server
    ServerThread server(a("ServerThread"), stack.listen(name, 10));

    // Perform one stupid connection with no data transfer
    {
        afl::base::Ptr<afl::net::Socket> socket;
        AFL_CHECK_SUCCEEDS(a("01. socket connect"), socket = stack.connect(name, 500).asPtr());
        a.checkNonNull("02. socket exists", socket.get());
    }

    // Perform multiple connections
    {
        const size_t N = 5;
        afl::base::Ptr<afl::net::Socket> sockets[N];
        for (size_t i = 0; i < N; ++i) {
            AFL_CHECK_SUCCEEDS(a("11. socket connect"), sockets[i] = stack.connect(name, 500).asPtr());
            a.checkNonNull("12. socket exists", sockets[i].get());
        }

        // Async boilerplate
        afl::async::Controller ctl;
        afl::async::SendOperation tx;
        afl::async::ReceiveOperation rx;

        // Send something
        static const uint8_t send1[] = "hi";
        for (size_t i = 0; i < N; ++i) {
            tx.setData(send1);
            a.check("13. send result", sockets[i]->send(ctl, tx, 500));
            a.check("14. send completed", tx.isCompleted());
        }

        // Receive something
        uint8_t receive1[10];
        for (size_t i = 0; i < N; ++i) {
            rx.setData(receive1);
            a.check("15. receive result", sockets[i]->receive(ctl, rx, 500));
            a.checkEqual("16. receive complete", rx.getNumReceivedBytes(), sizeof(send1));
            a.check("17. receive data", rx.getReceivedBytes().equalContent(send1));
        }

        // Send shutdown
        static const uint8_t send2[] = "q";
        for (size_t i = 0; i < N; ++i) {
            tx.setData(send2);
            a.check("21. send result", sockets[i]->send(ctl, tx, 500));
            a.check("22. send completed", tx.isCompleted());
        }

        // Receive shutdown
        for (size_t i = 0; i < N; ++i) {
            rx.setData(receive1);
            a.check("23. receive result", sockets[i]->receive(ctl, rx, 500));
            a.checkEqual("24. receive complete", rx.getNumReceivedBytes(), 0U);
        }
    }
}

/** Test stop(). */
AFL_TEST("afl.net.Server:stop", a)
{
    // Network stack
    afl::net::NetworkStack& stack = afl::net::NetworkStack::getInstance();
    afl::net::Name name("localhost", uint16_t(std::rand() % 30000 + 20000));

    // Server
    std::auto_ptr<ServerThread> pServer(new ServerThread(a("ServerThread"), stack.listen(name, 10)));

    // Connect
    afl::base::Ptr<afl::net::Socket> socket;
    AFL_CHECK_SUCCEEDS(a("socket connect"), socket = stack.connect(name, 500).asPtr());
    a.checkNonNull("socket exists", socket.get());

    // Transfer some data
    afl::async::Controller ctl;
    afl::async::SendOperation tx;
    tx.setData(afl::string::toBytes("foo"));
    a.check("send success", socket->send(ctl, tx, 500));

    afl::async::ReceiveOperation rx;
    uint8_t buf[10];
    rx.setData(buf);
    a.check("receive success", socket->receive(ctl, rx, 500));

    // We have proven that the server lives. Stop it.
    pServer.reset();
}
