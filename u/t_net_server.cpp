/**
  *  \file u/t_net_server.cpp
  *  \brief Test for afl::net::Server
  */

#include <memory>
#include "afl/net/server.hpp"

#include "t_net.hpp"
#include "afl/async/receiveoperation.hpp"
#include "afl/async/sendoperation.hpp"
#include "afl/net/name.hpp"
#include "afl/net/networkstack.hpp"
#include "afl/net/protocolhandler.hpp"
#include "afl/net/protocolhandlerfactory.hpp"
#include "afl/sys/thread.hpp"

namespace {
    // A protocol handler for testing.
    // - sending a packet containing "q" closes the connection
    // - everything else is echoed back
    class Handler : public afl::net::ProtocolHandler {
     public:
        Handler()
            : m_sent(true)
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
                TS_ASSERT(m_sent);
                TS_ASSERT(!bytes.empty());
                m_buffer.assign(bytes.unsafeData(), bytes.unsafeData() + bytes.size());
                m_sent = false;
            }

        virtual void handleSendTimeout(afl::base::ConstBytes_t /*unsentBytes*/)
            { }

        virtual void handleConnectionClose()
            { }

     private:
        std::vector<uint8_t> m_buffer;
        bool m_sent;
    };

    class Factory : public afl::net::ProtocolHandlerFactory {
     public:
        Handler* create()
            {
                return new Handler();
            }
    };

    class ServerThread {
     public:
        ServerThread(afl::base::Ref<afl::net::Listener> listener)
            : m_factory(),
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
void
TestNetServer::testIt()
{
    // Network stack
    afl::net::NetworkStack& stack = afl::net::NetworkStack::getInstance();
    afl::net::Name name("localhost", uint16_t(std::rand() % 30000 + 20000));

    // Server
    ServerThread server(stack.listen(name, 10));

    // Perform one stupid connection with no data transfer
    {
        afl::base::Ptr<afl::net::Socket> socket;
        TS_ASSERT_THROWS_NOTHING(socket = stack.connect(name, 500).asPtr());
        TS_ASSERT(socket.get() != 0);
    }

    // Perform multiple connections
    {
        const size_t N = 5;
        afl::base::Ptr<afl::net::Socket> sockets[N];
        for (size_t i = 0; i < N; ++i) {
            TS_ASSERT_THROWS_NOTHING(sockets[i] = stack.connect(name, 500).asPtr());
            TS_ASSERT(sockets[i].get() != 0);
        }

        // Async boilerplate
        afl::async::Controller ctl;
        afl::async::SendOperation tx;
        afl::async::ReceiveOperation rx;

        // Send something
        static const uint8_t send1[] = "hi";
        for (size_t i = 0; i < N; ++i) {
            tx.setData(send1);
            TS_ASSERT(sockets[i]->send(ctl, tx, 500));
            TS_ASSERT(tx.isCompleted());
        }

        // Receive something
        uint8_t receive1[10];
        for (size_t i = 0; i < N; ++i) {
            rx.setData(receive1);
            TS_ASSERT(sockets[i]->receive(ctl, rx, 500));
            TS_ASSERT_EQUALS(rx.getNumReceivedBytes(), sizeof(send1));
            TS_ASSERT(rx.getReceivedBytes().equalContent(send1));
        }

        // Send shutdown
        static const uint8_t send2[] = "q";
        for (size_t i = 0; i < N; ++i) {
            tx.setData(send2);
            TS_ASSERT(sockets[i]->send(ctl, tx, 500));
            TS_ASSERT(tx.isCompleted());
        }

        // Receive shutdown
        for (size_t i = 0; i < N; ++i) {
            rx.setData(receive1);
            TS_ASSERT(sockets[i]->receive(ctl, rx, 500));
            TS_ASSERT_EQUALS(rx.getNumReceivedBytes(), 0U);
        }
    }
}

/** Test stop(). */
void
TestNetServer::testStop()
{
    // Network stack
    afl::net::NetworkStack& stack = afl::net::NetworkStack::getInstance();
    afl::net::Name name("localhost", uint16_t(std::rand() % 30000 + 20000));

    // Server
    std::auto_ptr<ServerThread> pServer(new ServerThread(stack.listen(name, 10)));

    // Connect
    afl::base::Ptr<afl::net::Socket> socket;
    TS_ASSERT_THROWS_NOTHING(socket = stack.connect(name, 500).asPtr());
    TS_ASSERT(socket.get() != 0);

    // Transfer some data
    afl::async::Controller ctl;
    afl::async::SendOperation tx;
    tx.setData(afl::string::toBytes("foo"));
    TS_ASSERT(socket->send(ctl, tx, 500));

    afl::async::ReceiveOperation rx;
    uint8_t buf[10];
    rx.setData(buf);
    TS_ASSERT(socket->receive(ctl, rx, 500));

    // We have proven that the server lives. Stop it.
    pServer.reset();
}
