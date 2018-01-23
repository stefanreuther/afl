/**
  *  \file u/t_net_simpleserver.cpp
  *  \brief Test for afl::net::SimpleServer
  */

#include "afl/net/simpleserver.hpp"

#include <cstdio>
#include "u/t_net.hpp"
#include "afl/base/types.hpp"
#include "afl/net/protocolhandler.hpp"
#include "afl/net/protocolhandlerfactory.hpp"
#include "afl/net/networkstack.hpp"
#include "afl/net/name.hpp"
#include "afl/sys/thread.hpp"
#include "afl/async/controller.hpp"
#include "afl/async/sendoperation.hpp"
#include "afl/async/receiveoperation.hpp"

namespace {
    // A protocol handler for testing.
    // - sending a packet containing "q" closes the connection
    // - everything else is echoed back
    class Handler : public afl::net::ProtocolHandler {
     public:
        Handler()
            : m_buffer(),
              m_sent(true)
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
}

/** Simple test. */
void
TestNetSimpleServer::testIt()
{
    // Network stack
    afl::net::NetworkStack& stack = afl::net::NetworkStack::getInstance();

    // FIXME: there is no way to shut down a SimpleServer yet.
    // Therefore, create it on the heap and let it live forever.
    afl::net::Name name("localhost", uint16_t(std::rand() % 30000 + 20000));

    Factory f;
    afl::net::SimpleServer server(stack.listen(name, 10), f);
    afl::sys::Thread thread("SimpleServer", server);
    thread.start();

    // Perform one stupid connection with no data transfer
    {
        afl::base::Ptr<afl::net::Socket> socket;
        TS_ASSERT_THROWS_NOTHING(socket = stack.connect(name, 500).asPtr());
        TS_ASSERT(socket.get() != 0);
    }

    // Perform another connection
    {
        afl::base::Ptr<afl::net::Socket> socket;
        TS_ASSERT_THROWS_NOTHING(socket = stack.connect(name, 500).asPtr());
        TS_ASSERT(socket.get() != 0);

        // Async boilerplate
        afl::async::Controller ctl;
        afl::async::SendOperation tx;
        afl::async::ReceiveOperation rx;

        // Send something
        static const uint8_t send1[] = "hi";
        tx.setData(send1);
        TS_ASSERT(socket->send(ctl, tx, 500));
        TS_ASSERT(tx.isCompleted());

        // Receive something
        uint8_t receive1[10];
        rx.setData(receive1);
        TS_ASSERT(socket->receive(ctl, rx, 500));
        TS_ASSERT_EQUALS(rx.getNumReceivedBytes(), sizeof(send1));
        TS_ASSERT(rx.getReceivedBytes().equalContent(send1));

        // Request stop
        server.stop();

        // Send shutdown
        static const uint8_t send2[] = "q";
        tx.setData(send2);
        TS_ASSERT(socket->send(ctl, tx, 500));
        TS_ASSERT(tx.isCompleted());

        // Receive shutdown
        rx.setData(receive1);
        TS_ASSERT(socket->receive(ctl, rx, 500));
        TS_ASSERT_EQUALS(rx.getNumReceivedBytes(), 0U);
    }

    thread.join();
}
